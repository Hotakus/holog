/**
  ******************************************************************************
  * @file           : holog.h
  * @author         : Hotakus (hotakus@foxmail.com)
  * @brief          : None
  * @date           : 2024/4/4
  ******************************************************************************
  */

#ifndef HOLOG_HOLOG_H
#define HOLOG_HOLOG_H


#include <stdint.h>
#include <stdbool.h>
#include "homsg_psp.h"

#ifdef __cplusplus
extern "C" {
#endif
typedef enum holog_res_t {
    HOLOG_RES_OK = 0,
    HOLOG_RES_ERROR,
    HOLOG_RES_NO_INIT,
} holog_res_t;

typedef enum holog_level_t {
    HOLOG_LEVEL_NONE = 0x00,       // (0)  None
    HOLOG_LEVEL_INFO = 0x01,       // (1)  Info
    HOLOG_LEVEL_ERROR = 0x02,      // (2)  Error
    HOLOG_LEVEL_WARNING = 0x04,    // (4)  Warning
    HOLOG_LEVEL_FATAL = 0x08,      // (5)  Fatal
    HOLOG_LEVEL_DEBUG = 0x10,      // (16) Debug
    HOLOG_LEVEL_TRACE = 0x20,      // (32) Trace
    HOLOG_LEVEL_END,               // (0x21) End
} holog_level_t;

typedef struct holog_device_opr_t holog_device_opr_t;
typedef struct holog_device_opr_t {
    size_t (*timestamp)();
    homsg_subscriber_update_callback_t transmit_callback;
} holog_device_opr_t;

typedef struct holog_device_t holog_device_t;
typedef struct holog_device_t {
    const char *name;
    uint8_t level;  // default: HOLOG_INFO_TYPE_STDOUT. HOLOG_INFO_TYPE_INFO | HOLOG_INFO_TYPE_WARNING = 0x05
    holog_device_opr_t *opr;
} holog_device_t;

typedef struct holog_msg_t holog_msg_t;
typedef struct holog_msg_t {
    holog_device_t *dev;
    const char *text;
} holog_msg_t;

typedef struct holog_t holog_t;
typedef struct holog_t {
    homsg_psp_t *psp;

    struct {
        holog_device_t *(*create)(const char *name, holog_level_t level, holog_device_opr_t *opr);
        holog_res_t (*destroy)(holog_device_t *dev);
        holog_res_t (*register_device)(holog_device_t *dev);
        homsg_res_t (*unregister_device)(holog_device_t *dev);

        holog_res_t (*printf)(holog_level_t level, const char *fmt, ...);

        void (*set_level)(holog_device_t *dev, holog_level_t level);
        void (*set_opr_handle)(holog_device_t *dev, holog_device_opr_t *opr);

        void (*enable_level)(holog_device_t *dev, holog_level_t level);
        void (*disable_level)(holog_device_t *dev, holog_level_t level);
    };
} holog_t;

holog_t *holog();
holog_res_t holog_init();
holog_res_t holog_deinit();

#ifdef __cplusplus
}
#endif // __cplusplus

#endif  //HOLOG_HOLOG_H
