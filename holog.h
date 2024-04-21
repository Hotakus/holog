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
#include "holog_conf.h"


#ifdef __cplusplus
extern "C" {
#endif
typedef enum holog_res_t {
    HOLOG_RES_OK = 0,
    HOLOG_RES_ERROR,
    HOLOG_RES_NO_INIT,
    HOLOG_RES_INVALID_PARAMS,

    HOLOG_RES_FILE_OPEN_ERROR,
    HOLOG_RES_FILE_WRITE_ERROR,
    HOLOG_RES_FILE_READ_ERROR,
    HOLOG_RES_FILE_SEEK_ERROR,
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

typedef enum holog_device_type_t holog_device_type_t;
typedef enum holog_device_type_t {
    HOLOG_DEVICE_TYPE_STDOUT,
    HOLOG_DEVICE_TYPE_COMMON_FILE,
    HOLOG_DEVICE_TYPE_FATFS,
    HOLOG_DEVICE_TYPE_LITTLE_FS,
    HOLOG_DEVICE_TYPE_END
} holog_device_type_t;

typedef struct holog_style_t {
    const char *A;
    const char *B;
    const char *C;
    const char *D;
} holog_style_t;

typedef struct holog_msg_t holog_msg_t;
typedef struct holog_msg_t {
    holog_style_t style;
    const char *path;
    const char *linefeed;

#if (HOLOG_LITTLEFS_ENABLED == 1)
    lfs_t *lfs;
#endif

} holog_msg_t;

typedef struct holog_device_t holog_device_t;
typedef struct holog_device_t {
    const char *name;
    const char *linefeed;

    holog_device_type_t type;
    const char *log_path;

#if (HOLOG_LITTLEFS_ENABLED == 1)
    lfs_t *lfs;
#endif

    uint8_t level;
} holog_device_t;

typedef struct holog_t holog_t;
typedef struct holog_t {
    homsg_psp_t *psp;
    chain_t *devices;

    struct {
        holog_device_t *(*create)(const char *name, holog_device_type_t type, holog_level_t level, const char *linefeed);
        holog_res_t (*destroy)(holog_device_t *dev);

        holog_res_t (*register_device)(holog_device_t *dev);
        holog_res_t (*unregister_device)(holog_device_t *dev);

        holog_res_t (*printf)(holog_level_t level, char *file_path, char *file_name, int line, const char *fmt, ...);

        void (*set_level)(holog_device_t *dev, holog_level_t level);

        void (*enable_level)(holog_device_t *dev, holog_level_t level);
        void (*disable_level)(holog_device_t *dev, holog_level_t level);
        void (*mute)(holog_device_t *dev);

        // only for file device
        void (*set_log_path)(holog_device_t *dev, const char *log_path);
    };
} holog_t;

holog_t *holog();
holog_res_t holog_init();
holog_res_t holog_deinit();


#define HOLOG_LEVEL_ALL (HOLOG_LEVEL_INFO | HOLOG_LEVEL_ERROR | HOLOG_LEVEL_WARNING | HOLOG_LEVEL_FATAL | HOLOG_LEVEL_DEBUG | HOLOG_LEVEL_TRACE)

#define holog_debug(fmt, ...)   holog()->printf(HOLOG_LEVEL_DEBUG, __FILE__, __FILE_NAME__, __LINE__, fmt, ##__VA_ARGS__)
#define holog_trace(fmt, ...)   holog()->printf(HOLOG_LEVEL_TRACE, __FILE__, __FILE_NAME__, __LINE__, fmt, ##__VA_ARGS__)
#define holog_info(fmt, ...)    holog()->printf(HOLOG_LEVEL_INFO, __FILE__, __FILE_NAME__, __LINE__, fmt, ##__VA_ARGS__)
#define holog_error(fmt, ...)   holog()->printf(HOLOG_LEVEL_ERROR, __FILE__, __FILE_NAME__, __LINE__, fmt, ##__VA_ARGS__)
#define holog_warning(fmt, ...) holog()->printf(HOLOG_LEVEL_WARNING, __FILE__, __FILE_NAME__, __LINE__, fmt, ##__VA_ARGS__)
#define holog_fatal(fmt, ...)   holog()->printf(HOLOG_LEVEL_FATAL, __FILE__, __FILE_NAME__, __LINE__, fmt, ##__VA_ARGS__)


#ifdef __cplusplus
}
#endif // __cplusplus

#endif  //HOLOG_HOLOG_H
