/**
  ******************************************************************************
  * @file           : holog.c
  * @author         : Hotakus (hotakus@foxmail.com)
  * @brief          : None
  * @date           : 2024/4/4
  ******************************************************************************
  */


#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

#include "homsg_psp.h"
#include "holog_conf.h"
#include "holog.h"

static holog_log_level_t log_level_list[] = HOLOG_LOG_LEVEL_STYLE;
static const holog_style_list_t style_list[] = HOLOG_LOG_STYLE_LIST;

static holog_device_t *holog_dev_create(const char *name, holog_device_type_t type, holog_level_t level, const char *linefeed);
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
holog_device_t *holog_dev_create(const char *name, holog_device_type_t type, holog_level_t level, const char *linefeed) {
    holog_device_t *dev = HOLOG_MALLOC(sizeof(holog_device_t));

    dev->name = name;
    dev->type = type;
    dev->log_path = NULL;
    dev->level = level;
    dev->linefeed = linefeed;

    return dev;
}

/**
 * @brief
 * @param dev
 * @return
 */
holog_res_t holog_dev_destroy(holog_device_t *dev) {
    HOLOG_FREE(dev);
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
            subject_node = chain_find_node_by_name(instance.psp->all_subjects, log_level_list[k].level, true);
            if (subject_node == NULL) {
                subject = instance.psp->create_subject((char *)log_level_list[k].level);
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
            subject_node = chain_find_node_by_name(instance.psp->all_subjects, log_level_list[k].level, true);
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
    char *style_buf = HOLOG_MALLOC(HOLOG_PRINTF_MAX_SIZE);   // 格式化后的字符串最大长度
    if (style_buf == NULL) {
        return HOLOG_RES_ERROR;
    }

    // 通知相应的消息主题
    chain_node_t *subject_node = NULL;
    homsg_subject_t *subject = NULL;
    for (int i = 1, k = 0; i < HOLOG_LEVEL_END; i = i << 1, k++) {
        if (i == level) {
            subject_node = chain_find_node_by_name(instance.psp->all_subjects, log_level_list[k].level, true);
            if (subject_node == NULL) {
                HOLOG_FREE(style_buf);
                return HOLOG_RES_ERROR;
            }

            subject = (homsg_subject_t *)subject_node->data;
            if (subject == NULL) {
                HOLOG_FREE(style_buf);
                return HOLOG_RES_ERROR;
            }

            // 遍历所有订阅者
            chain_node_t *subscriber = subject->subscribers->head->next_node;
            if (subscriber == subject->subscribers->tail) {
                HOLOG_FREE(style_buf);
                return HOLOG_RES_ERROR;
            }
            while (subscriber != subject->subscribers->tail) {
                chain_node_t *registered_subscriber = chain_find_node_by_name(instance.devices, subscriber->name, true);
                if (registered_subscriber == NULL) {
                    HOLOG_FREE(style_buf);
                    return HOLOG_RES_ERROR;
                }

                holog_device_t *dev = (holog_device_t *)registered_subscriber->data;
                if (dev == NULL) {
                    HOLOG_FREE(style_buf);
                    return HOLOG_RES_ERROR;
                }

#if (HOLOG_USE_COLOR == 1)
                uint8_t pos_step = 32;
#else
                uint8_t pos_step = 16;
#endif
                uint16_t style_pos = (HOLOG_PRINTF_MAX_SIZE - 128);
                uint16_t date_pos = style_pos + 0;
                uint16_t time_pos = style_pos + pos_step * 1;
                uint16_t path_pos = style_pos + pos_step * 2;
                uint16_t type_pos = style_pos + pos_step * 3;

                time_t timestamp = HOLOG_GET_TIMESTAMP();
                struct tm *tm = localtime(&timestamp);

                // 风格化消息
                const char **style_p = ((const char **)(&dev->style.A));
                memset(style_buf, '\0', HOLOG_PRINTF_MAX_SIZE);
                for (int j = 0; j < sizeof(style_list) / sizeof(holog_style_list_t); ++j) {
                    switch (style_list[j].style) {
                        case HOLOG_STYLE_TIME : {
                            const char *time_fmt = NULL;
                            if (dev->type == HOLOG_DEVICE_TYPE_STDOUT && HOLOG_USE_COLOR) {
                                time_fmt = "%c"HOLOG_COLOR_BLUE"%02d:%02d:%02d"HOLOG_COLOR_NONE"%c";
                            } else {
                                time_fmt = "%c""%02d:%02d:%02d""%c";
                            }
                            sprintf(&style_buf[time_pos], time_fmt, style_list[j].bracket[0],
                                    tm->tm_hour, tm->tm_min, tm->tm_sec, style_list[j].bracket[1]);
                            style_p[j] = &style_buf[time_pos];
                            break;
                        }
                        case HOLOG_STYLE_DATE : {
                            const char *date_fmt = NULL;
                            if (dev->type == HOLOG_DEVICE_TYPE_STDOUT && HOLOG_USE_COLOR) {
                                date_fmt = "%c"HOLOG_COLOR_BLUE"%02d-%02d-%02d"HOLOG_COLOR_NONE"%c";
                            } else {
                                date_fmt = "%c""%02d-%02d-%02d""%c";
                            }
                            sprintf(&style_buf[date_pos], date_fmt, style_list[j].bracket[0],
                                    tm->tm_year - 100, tm->tm_mon + 1, tm->tm_mday, style_list[j].bracket[1]);
                            style_p[j] = &style_buf[date_pos];
                            break;
                        }
                        case HOLOG_STYLE_LEVEL : {
                            const char *level_fmt = NULL;
                            if (dev->type == HOLOG_DEVICE_TYPE_STDOUT && HOLOG_USE_COLOR) {
                                level_fmt = "%c""%s""%s"HOLOG_COLOR_NONE"%c";
                                sprintf(&style_buf[type_pos], level_fmt, style_list[j].bracket[0],
                                        log_level_list[k].color, log_level_list[k].level, style_list[j].bracket[1]);
                            } else {
                                level_fmt = "%c""%s""%c";
                                sprintf(&style_buf[type_pos], level_fmt, style_list[j].bracket[0],
                                        log_level_list[k].level, style_list[j].bracket[1]);
                            }
                            style_p[j] = &style_buf[type_pos];
                            break;
                        }
                        case HOLOG_STYLE_MAIN_CONTENT : {
                            // 格式化可变参数列表
                            va_list args;
                            va_start(args, fmt);
                            vsnprintf(&style_buf[1], style_pos, fmt, args);
                            va_end(args);

                            style_buf[0] = style_list[j].bracket[0];
                            style_buf[strlen(style_buf)] = style_list[j].bracket[1];
//                            style_buf[strlen(style_buf) + 1] = '\0';

                            style_p[j] = &style_buf[0];
                            break;
                        }
                        case HOLOG_STYLE_FILE_NAME : {
                            const char *file_fmt = NULL;
                            if (dev->type == HOLOG_DEVICE_TYPE_STDOUT && HOLOG_USE_COLOR) {
                                file_fmt = "%c"HOLOG_COLOR_DARY_GRAY"%s:%d"HOLOG_COLOR_NONE"%c";
                            } else {
                                file_fmt = "%c""%s:%d""%c";
                            }
                            sprintf(&style_buf[path_pos], file_fmt, style_list[j].bracket[0],
                                    file_name, line, style_list[j].bracket[1]);
                            style_p[j] = &style_buf[path_pos];
                            break;
                        }
                        case HOLOG_STYLE_FILE_PATH : {
                            if (dev->type == HOLOG_DEVICE_TYPE_STDOUT && HOLOG_USE_COLOR) {
                                sprintf(&style_buf[path_pos], HOLOG_COLOR_DARY_GRAY"%s:%d"HOLOG_COLOR_NONE, file_path, line);
                            } else {
                                sprintf(&style_buf[path_pos], "%s:%d", file_path, line);
                            }
                            style_p[j] = &style_buf[path_pos];
                            break;
                        }
                        case HOLOG_STYLE_FILE_RELATIVE_PATH: {
                            // todo)):
                            style_p[j] = style_p[j];
                            break;
                        }
                        default: {
                            break;
                        }
                    }
                }

                homsg_subscriber_update_callback_t update = (homsg_subscriber_update_callback_t)subscriber->data;
                update((void *)dev);    // Run callback
                subscriber = subscriber->next_node;
            }
            break;
        }
    }

    HOLOG_FREE(style_buf);
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
#if (HOLOG_STDOUT_ENABLED == 1)
    holog_device_t *msg = (holog_device_t *)params;
    printf("%s""%s""%s""%s""%s", msg->style.A, msg->style.B, msg->style.C, msg->style.D, msg->linefeed);
#endif
}

#if (HOLOG_AUTO_FILE_CREATE_BY_DATE == 1)
static char *auto_file_name_create(const char *root) {
    time_t t = HOLOG_GET_TIMESTAMP();
    struct tm *tm = localtime(&t);

    uint8_t path_len = strlen(root) + 32;
    char *name = HOLOG_MALLOC(path_len);
    sprintf(name, "%s"HOLOG_AUTO_FILE_FORMAT, root, tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday);
    return name;
}
#endif

void holog_common_file_callback(void *params) {
#if (HOLOG_COMMON_FILE_ENABLED == 1)
    holog_device_t *msg = (holog_device_t *)params;
    const char *path = NULL;

#if (HOLOG_AUTO_FILE_CREATE_BY_DATE == 1)
    path = auto_file_name_create(msg->log_path);
#else
    path = msg->log_path;
#endif

    FILE *fp = fopen(path, "a+");
    if (fp == NULL) {
        return;
    }

    // judge empty
    fseek(fp, 0, SEEK_END);
    if (ftell(fp) == 0) {
        fprintf(fp, "%s""%s", HOLOG_BANNER, msg->linefeed);
    }

    fseek(fp, 0, SEEK_END);
    fprintf(fp, "%s""%s""%s""%s""%s", msg->style.A, msg->style.B, msg->style.C, msg->style.D, msg->linefeed);

    fclose(fp);

#if (HOLOG_AUTO_FILE_CREATE_BY_DATE == 1)
    HOLOG_FREE((void *)path);
#endif
#endif
}

void holog_fatfs_callback(void *params) {
#if (HOLOG_FATFS_ENABLED == 1)
    holog_device_t *msg = (holog_device_t *)params;
    FIL fil;
    FILINFO fno;
    const char *path = NULL;

#if (HOLOG_AUTO_FILE_CREATE_BY_DATE == 1)
    path = auto_file_name_create(msg->log_path);
#else
    path = msg->log_path;
#endif

    f_stat(path, &fno);

    FRESULT res = f_open(&fil, path, FA_READ | FA_WRITE | FA_CREATE_ALWAYS);
    if (res != FR_OK) {
        return;
    }

    if (fno.fsize == 0) {
        f_printf(&fil, "%s\n", HOLOG_BANNER);
    }

    f_lseek(&fil, fno.fsize);
    f_printf(&fil, "%s""%s""%s""%s""%s", msg->style.A, msg->style.B, msg->style.C, msg->style.D, msg->linefeed);

    f_close(&fil);

#if (HOLOG_AUTO_FILE_CREATE_BY_DATE == 1)
    HOLOG_FREE((void *)path);
#endif
#endif
}

// todo)): littlefs
void holog_littlefs_callback(void *params) {
#if (HOLOG_LITTLEFS_ENABLED == 1)
    holog_device_t *msg = (holog_device_t *)params;
    const char *path = NULL;

    enum lfs_error err;
    lfs_file_t file;

#if (HOLOG_AUTO_FILE_CREATE_BY_DATE == 1)
    path = auto_file_name_create(msg->log_path);
#else
    path = msg->log_path;
#endif

    err = lfs_file_open(msg->lfs, &file, path, LFS_O_RDWR | LFS_O_CREAT);
    if (err != LFS_ERR_OK) {
        return;
    }

    if (lfs_file_size(msg->lfs, &file) == 0) {
        err = lfs_file_write(msg->lfs, &file, HOLOG_BANNER, strlen(HOLOG_BANNER));
        if (err != LFS_ERR_OK) {
            return;
        }
    }

    err = lfs_file_seek(msg->lfs, &file, 0, LFS_SEEK_END);
    if (err != LFS_ERR_OK) {
        return;
    }
    const char **style_p = ((const char **)(&msg->style.A));
    for (int i = 0; i < sizeof(holog_style_t) / sizeof(msg->style.A); ++i) {
        err = lfs_file_write(msg->lfs, &file, style_p[i], strlen(style_p[i]));
        if (err != LFS_ERR_OK) {
            return;
        }
//        lfs_file_write(msg->lfs, &file, " ", 1);
    }

    err = lfs_file_write(msg->lfs, &file, msg->linefeed, strlen(msg->linefeed));
    if (err != LFS_ERR_OK) {
        return;
    }

    lfs_file_close(msg->lfs, &file);

#if (HOLOG_AUTO_FILE_CREATE_BY_DATE == 1)
    HOLOG_FREE((void *)path);
#endif
#endif
}
