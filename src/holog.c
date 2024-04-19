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
#include <fcntl.h>  // POSIX系统下的文件操作函数
#include <unistd.h>

//
const char *log_string[] = {"INFO", "ERROR", "WARNING", "FATAL", "DEBUG", "TRACE"};

static holog_device_t *holog_dev_create(const char *name, holog_device_type_t type, holog_level_t level);
static holog_res_t holog_dev_destroy(holog_device_t *dev);

static holog_res_t holog_register_device(holog_device_t *dev);
static holog_res_t unregister_device(holog_device_t *dev);

static holog_res_t holog_printf(holog_level_t level, const char *fmt, ...);

static void holog_set_level(holog_device_t *dev, holog_level_t level);

static void enable_level(holog_device_t *dev, holog_level_t level);
static void disable_level(holog_device_t *dev, holog_level_t level);
static void mute(holog_device_t *dev);

static void set_log_path(holog_device_t *dev, const char *log_path);


static void holog_stdout_callback(void *params);
static void holog_common_file_callback(void *params);
static void holog_fatfs_callback(void *params);
static void holog_littlefs_callback(void *params);

#define HOLOG_BANNER "[Recorded by HoLog]"

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

holog_device_t *holog_dev_create(const char *name, holog_device_type_t type, holog_level_t level) {
    holog_device_t *dev = malloc(sizeof(holog_device_t));

    dev->name = name;

    dev->type = type;
    dev->log_path = NULL;

    dev->level = level;

    return dev;
}

holog_res_t holog_dev_destroy(holog_device_t *dev) {
    free(dev);
    return HOLOG_RES_OK;
}

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
            subject_node = chain_find_node_by_name(instance.psp->all_subjects, log_string[k], true);
            if (subject_node == NULL) {
                subject = instance.psp->create_subject((char *)log_string[k]);
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
            subject_node = chain_find_node_by_name(instance.psp->all_subjects, log_string[k], true);
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

holog_res_t holog_printf(holog_level_t level, const char *fmt, ...) {
    va_list args;
    char buffer[HOLOG_PRINTF_MAX_SIZE]; // 格式化后的字符串最大长度

    // 格式化可变参数列表
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    // 通知相应的消息主题
    chain_node_t *subject_node = NULL;
    homsg_subject_t *subject = NULL;
    for (int i = 1, k = 0; i < HOLOG_LEVEL_END; i = i << 1, k++) {
        if (i == level) {
            subject_node = chain_find_node_by_name(instance.psp->all_subjects, log_string[k], true);
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

                holog_msg_t msg;
                msg.text = (char *)buffer;
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

void holog_set_level(holog_device_t *dev, holog_level_t level) {
    dev->level = level;
}

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
    printf("msg->text: %s", msg->text);
    printf("msg->path: %s", msg->path);
}

void holog_common_file_callback(void *params) {
    holog_msg_t *msg = (holog_msg_t *)params;
    printf("common file msg->text: %s", msg->text);

    int fd = open(msg->path, O_CREAT | O_RDWR);
    if (fd < 0) {
        printf("open %s failed\n", msg->path);
        return;
    }


    // 将文件指针移动到文件末尾
    if (lseek(fd, 0, SEEK_END) == -1) {
        printf("lseek %s failed 1\n", msg->path);
        close(fd);
        return;
    } else {
        if (tell(fd) == 0) {
            write(fd, HOLOG_BANNER"\n", sizeof(HOLOG_BANNER));
        } else {
            // 检查文件的最后一个字符是否是换行符
            char last_char;
            if (lseek(fd, -1, SEEK_END) == -1) {
                printf("lseek %s failed 2\n", msg->path);
                close(fd);
                return;
            }
            if (read(fd, &last_char, 1) == -1) {
                printf("read %s failed\n", msg->path);
                close(fd);
                return;
            }

            // 如果最后一个字符不是换行符，则在写入内容之前添加一个换行符
            if (last_char != '\n') {
                if (write(fd, "\n", 1) == -1) {
                    printf("write %s failed 1\n", msg->path);
                    close(fd);
                    return;
                }
            }
        }
    }

    // 写入内容
    lseek(fd, 0, SEEK_END);
    if (write(fd, msg->text, strlen(msg->text)) == -1) {
        printf("write %s failed 2\n", msg->path);
        close(fd);
        return;
    }

    close(fd);
}

void holog_fatfs_callback(void *params) {

}

void holog_littlefs_callback(void *params) {

}

int main() {
    holog_init();

    holog_device_t *dev = holog()->create("test_dev", HOLOG_DEVICE_TYPE_COMMON_FILE, HOLOG_LEVEL_INFO | HOLOG_LEVEL_WARNING);
    holog()->set_log_path(dev, "./test.log");
    holog()->register_device(dev);

    holog()->printf(HOLOG_LEVEL_INFO, "test( %s , %s , %d , %s )", __DATE__, __FILE_NAME__, __LINE__, __FUNCTION__);

    return 0;
}
