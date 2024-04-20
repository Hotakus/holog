/**
  ******************************************************************************
  * @file           : holog.c
  * @author         : Hotakus (hotakus@foxmail.com)
  * @brief          : None
  * @date           : 2024/4/4
  ******************************************************************************
  */

#include "holog.h"
#include <stdio.h>
#include "homsg_psp.h"
#include "holog_conf.h"
#include <stdarg.h>
#include <string.h>


#define HOLOG_BANNER "[Recorded by HoLog]"

typedef struct holog_log_string_t {
    const char *level;
    const char *color;
} holog_log_string_t;

static holog_log_string_t log_string_list[] = {
        {"INFO",    HOLOG_COLOR_CYAN},
        {"ERROR",   HOLOG_COLOR_RED},
        {"WARNING", HOLOG_COLOR_YELLOW},
        {"FATAL",   HOLOG_COLOR_LIGHT_RED},
        {"DEBUG",   HOLOG_COLOR_DARY_GRAY},
        {"TRACE",   HOLOG_COLOR_BROWN}
};

//static const char *log_string[] = {
//        "INFO", "ERROR", "WARNING", "FATAL", "DEBUG", "TRACE"
//};
const uint8_t style_list[] = HOLOG_LOG_STYLE_LIST;

static holog_device_t *holog_dev_create(const char *name, holog_device_type_t type, holog_level_t level);
static holog_res_t holog_dev_destroy(holog_device_t *dev);

static holog_res_t holog_register_device(holog_device_t *dev);
static holog_res_t unregister_device(holog_device_t *dev);

static holog_res_t holog_printf(holog_level_t level, char *file_path, char *file_name, int line, const char *fmt, ...);

static void holog_set_level(holog_device_t *dev, holog_level_t level);

static void enable_level(holog_device_t *dev, holog_level_t level);
static void disable_level(holog_device_t *dev, holog_level_t level);
static void mute(holog_device_t *dev);

static void set_log_path(holog_device_t *dev, const char *log_path);


static void holog_stdout_callback(void *params);
static void holog_common_file_callback(void *params);
static void holog_fatfs_callback(void *params);
static void holog_littlefs_callback(void *params);


static holog_t instance = {
        .psp = NULL,
        .create = holog_dev_create,
        .destroy = holog_dev_destroy,
        .register_device = holog_register_device,
        .unregister_device = unregister_device,
        .printf = holog_printf,
        .set_level = holog_set_level,
        .enable_level = enable_level,
        .disable_level = disable_level,
        .mute = mute,
        .set_log_path = set_log_path
};

holog_t *holog() {
    return &instance;
}

holog_res_t holog_init() {
    if (instance.psp == NULL) {
        instance.psp = homsg_psp_create();
        if (instance.psp == NULL) {
            return HOLOG_RES_ERROR;
        }
    }

    if (instance.devices == NULL) {
        instance.devices = chain_create("devices");
        if (instance.devices == NULL) {
            return HOLOG_RES_ERROR;
        }
    }

    return HOLOG_RES_OK;
}

holog_res_t holog_deinit() {
    if (instance.psp == NULL) {
        return HOLOG_RES_ERROR;
    }
    homsg_psp_destroy(instance.psp);

    if (instance.devices == NULL) {
        return HOLOG_RES_ERROR;
    }
    chain_destroy(instance.devices);

    return HOLOG_RES_OK;
}

/**
 * @brief
 * @param name
 * @param type
 * @param level
 * @return
 */
holog_device_t *holog_dev_create(const char *name, holog_device_type_t type, holog_level_t level) {
    holog_device_t *dev = malloc(sizeof(holog_device_t));

    dev->name = name;

    dev->type = type;
    dev->log_path = NULL;

    dev->level = level;

    return dev;
}

/**
 * @brief
 * @param dev
 * @return
 */
holog_res_t holog_dev_destroy(holog_device_t *dev) {
    free(dev);
    return HOLOG_RES_OK;
}

/**
 * @brief
 * @param dev
 * @return
 */
holog_res_t holog_register_device(holog_device_t *dev) {
    if (dev == NULL) {
        return HOLOG_RES_ERROR;
    }

    if (instance.psp == NULL) {
        return HOLOG_RES_NO_INIT;
    }

    chain_node_t *device_node = chain_find_node_by_name(instance.devices, dev->name, true);
    if (device_node != NULL) {
        return HOLOG_RES_OK;
    }

    chain_node_t *subject_node = NULL;
    homsg_subject_t *subject = NULL;
    for (int i = 1, k = 0; i < HOLOG_LEVEL_END; i = i << 1, k++) {
        if ((dev->level & i) == i) {
            subject_node = chain_find_node_by_name(instance.psp->all_subjects, log_string_list[k].level, true);
            if (subject_node == NULL) {
                subject = instance.psp->create_subject((char *)log_string_list[k].level);
                instance.psp->publish(instance.psp, subject);
            } else {
                subject = (homsg_subject_t *)subject_node->data;
                if (subject == NULL) {
                    return HOLOG_RES_ERROR;
                }
            }

            homsg_subscriber_update_callback_t callback = NULL;
            switch (dev->type) {
                case HOLOG_DEVICE_TYPE_STDOUT: {
                    callback = holog_stdout_callback;
                    break;
                }
                case HOLOG_DEVICE_TYPE_COMMON_FILE: {
                    callback = holog_common_file_callback;
                    break;
                }
                case HOLOG_DEVICE_TYPE_FATFS: {
                    callback = holog_fatfs_callback;
                    break;
                }
                case HOLOG_DEVICE_TYPE_LITTLE_FS: {
                    callback = holog_littlefs_callback;
                    break;
                }
                default: {
                    return HOLOG_RES_INVALID_PARAMS;
                }
            }
            instance.psp->subscribe(instance.psp, subject, (char *)dev->name, callback);
        }
    }

    device_node = chain_node_create(dev->name, dev);
    if (device_node == NULL) {
        instance.psp->unsubscribe(instance.psp, subject, (char *)dev->name);
        return HOLOG_RES_ERROR;
    }
    chain_append(instance.devices, device_node);

    return HOLOG_RES_OK;
}

/**
 * @brief
 * @param dev
 * @return
 */
holog_res_t unregister_device(holog_device_t *dev) {
    if (dev == NULL) {
        return HOLOG_RES_ERROR;
    }

    if (instance.psp == NULL) {
        return HOLOG_RES_NO_INIT;
    }

    chain_node_t *device_node = chain_find_node_by_name(instance.devices, dev->name, true);
    if (device_node == NULL) {
        return HOLOG_RES_OK;
    }
    chain_remove_node(instance.devices, dev->name, NULL, false);

    chain_node_t *subject_node = NULL;
    homsg_subject_t *subject = NULL;
    for (int i = 1, k = 0; i < HOLOG_LEVEL_END; i = i << 1, k++) {
        if ((dev->level & i) == i) {
            subject_node = chain_find_node_by_name(instance.psp->all_subjects, log_string_list[k].level, true);
            if (subject_node == NULL) {
                return HOLOG_RES_ERROR;
            }

            subject = (homsg_subject_t *)subject_node->data;
            if (subject == NULL) {
                return HOLOG_RES_ERROR;
            }

            instance.psp->unsubscribe(instance.psp, subject, (char *)dev->name);
        }
    }

    return HOLOG_RES_OK;
}


holog_res_t holog_printf(holog_level_t level, char *file_path, char *file_name, int line, const char *fmt, ...) {
    va_list args;
    char style_buf[HOLOG_PRINTF_MAX_SIZE]; // 格式化后的字符串最大长度

    // 格式化可变参数列表
    va_start(args, fmt);
    vsnprintf(style_buf, sizeof(style_buf), fmt, args);
    va_end(args);

    // 通知相应的消息主题
    chain_node_t *subject_node = NULL;
    homsg_subject_t *subject = NULL;
    for (int i = 1, k = 0; i < HOLOG_LEVEL_END; i = i << 1, k++) {
        if (i == level) {
            subject_node = chain_find_node_by_name(instance.psp->all_subjects, log_string_list[k].level, true);
            if (subject_node == NULL) {
                return HOLOG_RES_ERROR;
            }

            subject = (homsg_subject_t *)subject_node->data;
            if (subject == NULL) {
                return HOLOG_RES_ERROR;
            }

            // 遍历所有订阅者
            chain_node_t *subscriber = subject->subscribers->head->next_node;
            if (subscriber == subject->subscribers->tail) {
                return HOLOG_RES_ERROR;
            }
            while (subscriber != subject->subscribers->tail) {
                chain_node_t *registered_subscriber = chain_find_node_by_name(instance.devices, subscriber->name, true);
                if (registered_subscriber == NULL) {
                    return HOLOG_RES_ERROR;
                }

                holog_device_t *dev = (holog_device_t *)registered_subscriber->data;
                if (dev == NULL) {
                    return HOLOG_RES_ERROR;
                }

                // 风格化消息
                holog_msg_t msg;
                const char **style_p = ((const char **)(&msg.style.A));
#if (HOLOG_USE_COLOR == 1)
                uint8_t pos_step = 32;
#else
                uint8_t pos_step = 16;
#endif
                uint16_t buf_half = (sizeof(style_buf) >> 1);
                uint16_t date_pos = buf_half + 0;
                uint16_t time_pos = buf_half + pos_step * 1;
                uint16_t path_pos = buf_half + pos_step * 2;
                uint16_t type_pos = buf_half + pos_step * 3;

                time_t timestamp = HOLOG_GET_TIMESTAMP();
                struct tm *tm = localtime(&timestamp);

                for (int j = 0; j < sizeof(style_list); ++j) {
                    switch (style_list[j]) {
                        case HOLOG_STYLE_TIME : {
                            if (dev->type == HOLOG_DEVICE_TYPE_STDOUT && HOLOG_USE_COLOR) {
                                strftime(&style_buf[time_pos], pos_step, HOLOG_COLOR_BLUE"%H:%M:%S"HOLOG_COLOR_NONE, tm);
                            } else {
                                strftime(&style_buf[time_pos], pos_step, "%H:%M:%S", tm);
                            }
                            style_p[j] = &style_buf[time_pos];
                            break;
                        }
                        case HOLOG_STYLE_DATE : {
                            if (dev->type == HOLOG_DEVICE_TYPE_STDOUT && HOLOG_USE_COLOR) {
                                strftime(&style_buf[date_pos], pos_step, HOLOG_COLOR_BLUE"%Y-%m-%d"HOLOG_COLOR_NONE, tm);
                            } else {
                                strftime(&style_buf[date_pos], pos_step, "%Y-%m-%d", tm);
                            }
                            style_p[j] = &style_buf[date_pos];
                            break;
                        }
                        case HOLOG_STYLE_TYPE : {
                            if (dev->type == HOLOG_DEVICE_TYPE_STDOUT && HOLOG_USE_COLOR) {
                                sprintf(&style_buf[type_pos], "%s""%s"HOLOG_COLOR_NONE, log_string_list[k].color, log_string_list[k].level);
                                style_p[j] = &style_buf[type_pos];
                            } else {
                                style_p[j] = log_string_list[k].level;
                            }
                            break;
                        }
                        case HOLOG_STYLE_MAIN_CONTENT : {
                            style_p[j] = &style_buf[0];
                            break;
                        }
                        case HOLOG_STYLE_FILE_NAME : {
                            sprintf(&style_buf[path_pos], "%s:%d:", file_name, line);
                            style_p[j] = &style_buf[path_pos];
                            break;
                        }
                        case HOLOG_STYLE_FILE_PATH : {
                            sprintf(&style_buf[path_pos], "%s:%d:", file_path, line);
                            style_p[j] = &style_buf[path_pos];
                            break;
                        }
                    }
                }

                msg.path = dev->log_path;

                homsg_subscriber_update_callback_t update = (homsg_subscriber_update_callback_t)subscriber->data;
                update(&msg);    // Run callback
                subscriber = subscriber->next_node;
            }
            break;
        }
    }

    return HOLOG_RES_OK;
}

/**
 * @brief
 * @param dev
 * @param level
 */
void holog_set_level(holog_device_t *dev, holog_level_t level) {
    dev->level = level;
}

/**
 * @brief
 * @param dev
 * @param level
 */
void enable_level(holog_device_t *dev, holog_level_t level) {
    if (dev == NULL) {
        return;
    }
    dev->level |= level;
}


void disable_level(holog_device_t *dev, holog_level_t level) {
    if (dev == NULL) {
        return;
    }
    dev->level &= ~level;
}


void mute(holog_device_t *dev) {
    if (dev == NULL) {
        return;
    }
    dev->level = HOLOG_LEVEL_NONE;
}


void set_log_path(holog_device_t *dev, const char *log_path) {
    if (dev == NULL) {
        return;
    }

    dev->log_path = log_path;
}


void holog_stdout_callback(void *params) {
    holog_msg_t *msg = (holog_msg_t *)params;
    printf("%s" " " "%s" " " "%s" " " "%s" " " "%s",
           msg->style.A, msg->style.B, msg->style.C, msg->style.D, HOLOG_LINEFEED);
}

void holog_common_file_callback(void *params) {
    holog_msg_t *msg = (holog_msg_t *)params;

#if (HOLOG_COMMON_FILE_ENABLED == 1)
    FILE *fp = fopen(msg->path, "a+");
    if (fp == NULL) {
        return;
    }

    // judge empty
    fseek(fp, 0, SEEK_END);
    if (ftell(fp) == 0) {
        fprintf(fp, "%s\n", HOLOG_BANNER);
    } else {
        // judge last char
        fseek(fp, -1, SEEK_END);
        if (fgetc(fp) != '\n') {
            fprintf(fp, "%s", HOLOG_LINEFEED);
        }
    }

    fseek(fp, 0, SEEK_END);
    fprintf(fp, "%s" " " "%s" " " "%s" " " "%s" " " "%s",
            msg->style.A, msg->style.B, msg->style.C, msg->style.D, HOLOG_LINEFEED);

    fclose(fp);
#endif
}

void holog_fatfs_callback(void *params) {
#if (HOLOG_FATFS_ENABLED == 1)

#endif
}

void holog_littlefs_callback(void *params) {
#if (HOLOG_LITTLEFS_ENABLED == 1)

#endif
}
