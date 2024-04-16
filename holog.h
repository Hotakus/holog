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
#ifdef __cplusplus
extern "C" {
#endif
typedef enum holog_res_t {
    HOLOG_RES_OK = 0,
    HOLOG_RES_ERROR
} holog_res_t;

typedef enum holog_output_t {
    HOLOG_OUTPUT_NONE = 0,
    HOLOG_OUTPUT_STDOUT,
    HOLOG_OUTPUT_UART,
    HOLOG_OUTPUT_FILE,
} holog_output_t;

typedef enum holog_info_type_t {
    HOLOG_INFO_TYPE_NONE = 0x00,       // (0)  None
    HOLOG_INFO_TYPE_INFO = 0x01,       // (1)  Info
    HOLOG_INFO_TYPE_ERROR = 0x02,      // (2)  Error
    HOLOG_INFO_TYPE_WARNING = 0x04,    // (4)  Warning
    HOLOG_INFO_TYPE_FATAL = 0x08,      // (5)  Fatal
    HOLOG_INFO_TYPE_DEBUG = 0x10,      // (16) Debug
    HOLOG_INFO_TYPE_TRACE = 0x20,      // (32) Trace
} holog_info_type_t;

typedef struct holog_device_opr_t holog_device_opr_t;
typedef struct holog_device_opr_t {
    holog_res_t (*)
} holog_device_opr_t;

typedef struct holog_device_t holog_device_t;
typedef struct holog_device_t {
    holog_output_t out_type;
    uint8_t info_type;          // default: HOLOG_INFO_TYPE_STDOUT. HOLOG_INFO_TYPE_INFO | HOLOG_INFO_TYPE_WARNING = 0x05

    holog_device_opr_t *opr;
} holog_device_t;

typedef struct holog_t holog_t;
typedef struct holog_t {

} holog_t;

holog_t *holog_create();
holog_res_t holog_destroy(holog_t *holog);

holog_device_t * holog_dev_create(holog_output_t out_type, uint8_t info_type);
holog_res_t holog_dev_destroy(holog_device_t *dev);
holog_res_t holog_dev_set_opr(holog_device_t *dev, holog_device_opr_t *opr);

void holog_dev_log(holog_device_t *dev, const char *msg);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif  //HOLOG_HOLOG_H
