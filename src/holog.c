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

const char *log_string[] = {"info", "error", "warning", "fatal", "debug", "trace"};

static holog_device_t *holog_dev_create(const char *name, holog_level_t level, holog_device_opr_t *opr);
static holog_res_t holog_dev_destroy(holog_device_t *dev);

static holog_res_t holog_register_device(holog_device_t *dev);

static holog_res_t holog_printf(holog_level_t level, const char *fmt, ...);

static void holog_set_level(holog_device_t *dev, holog_level_t level);
static void holog_set_opr_handle(holog_device_t *dev, holog_device_opr_t *opr);

static void enable_level(holog_device_t *dev, holog_level_t level);
static void disable_level(holog_device_t *dev, holog_level_t level);

static holog_t instance = {
        .psp = NULL,
        .create = holog_dev_create,
        .destroy = holog_dev_destroy,
        .register_device = holog_register_device,
        .printf = holog_printf,
        .set_level = holog_set_level,
        .set_opr_handle = holog_set_opr_handle,
        .enable_level = enable_level,
        .disable_level = disable_level,
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

    return HOLOG_RES_OK;
}

holog_res_t holog_deinit() {
    if (instance.psp == NULL) {
        return HOLOG_RES_ERROR;
    }
    homsg_psp_destroy(instance.psp);
    return HOLOG_RES_OK;
}

holog_device_t *holog_dev_create(const char *name, holog_level_t level, holog_device_opr_t *opr) {
    holog_device_t *dev = malloc(sizeof(holog_device_t));

    dev->name = name;
    dev->level = level;
    dev->opr = opr;

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

            instance.psp->subscribe(instance.psp, subject, (char *)dev->name, dev->opr->transmit_callback);
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

    // 通知主题
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
            homsg_res_t res = instance.psp->notify(subject, buffer);
            if (res != HOMSG_RES_OK) {
                return HOLOG_RES_ERROR;
            }

            break;
        }
    }

    return HOLOG_RES_OK;
}

void holog_set_level(holog_device_t *dev, holog_level_t level) {
    dev->level = level;
}

void holog_set_opr_handle(holog_device_t *dev, holog_device_opr_t *opr) {
    dev->opr = opr;
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


void test_transmit_callback(void *params) {
    char *msg = (char *)params;
    printf("holog_callback: %s\n\r", msg);
}

size_t test_timestamp() {
    size_t timstamp = 0;
    // turn timestamp to string


    return 0;
}

int main() {

    holog_init();

    holog_device_opr_t opr;
    opr.timestamp = NULL;
    opr.transmit_callback = test_transmit_callback;
    holog_device_t *dev = holog()->create("test_dev", HOLOG_LEVEL_INFO | HOLOG_LEVEL_WARNING, &opr);
    holog()->register_device(dev);

    holog()->printf(HOLOG_LEVEL_INFO, "test(%s)\r\n", "2gsdf");
    holog()->printf(HOLOG_LEVEL_WARNING, "hello world\r\n");

    return 0;
}
