/**
  ******************************************************************************
  * @file           : holog_conf.h
  * @author         : Hotakus (hotakus@foxmail.com)
  * @brief          : None
  * @date           : 2024/4/7
  ******************************************************************************
  */

#ifndef HOLOG_HOLOG_CONF_H
#define HOLOG_HOLOG_CONF_H

#define HOLOG_PRINTF_MAX_SIZE       (256)
#define HOLOG_LINEFEED              "\n"

#define HOLOG_STDOUT_ENABLED        (1)
#define HOLOG_COMMON_FILE_ENABLED   (1)
#define HOLOG_FATFS_ENABLED         (0)
#define HOLOG_LITTLEFS_ENABLED      (0)

#if (HOLOG_FATFS_ENABLED == 1)
#include "fatfs.h"
#endif
#if (HOLOG_LITTLEFS_ENABLED == 1)
#include "littlefs.h"
#endif

#define HOLOG_GET_TIMESTAMP()       (time(NULL))


// define log style
//#define HOLOG_STYLE_TYPE                (0u)
//#define HOLOG_STYLE_TIME                (1u)
//#define HOLOG_STYLE_DATE                (2u)
//#define HOLOG_STYLE_FILE_NAME           (3u)
//#define HOLOG_STYLE_FILE_PATH           (4u)
//#define HOLOG_STYLE_FILE_RELATIVE_PATH  (5u)
//#define HOLOG_STYLE_FILE_ROW_COLUMN     (6u)
//#define HOLOG_STYLE_MAIN_CONTENT        (7u)

typedef enum holog_style_define_t {
    HOLOG_STYLE_TYPE,
    HOLOG_STYLE_TIME,
    HOLOG_STYLE_DATE,
    HOLOG_STYLE_FILE_NAME,
    HOLOG_STYLE_FILE_PATH,
    HOLOG_STYLE_FILE_RELATIVE_PATH,
    HOLOG_STYLE_MAIN_CONTENT,
    HOLOG_STYLE_END
} holog_style_define_t;

#define HOLOG_STYLE_A                   (HOLOG_STYLE_TIME)
#define HOLOG_STYLE_B                   (HOLOG_STYLE_TYPE)
#define HOLOG_STYLE_C                   (HOLOG_STYLE_FILE_NAME)
#define HOLOG_STYLE_D                   (HOLOG_STYLE_MAIN_CONTENT)
#define HOLOG_LOG_STYLE_LIST            {HOLOG_STYLE_A, HOLOG_STYLE_B, HOLOG_STYLE_C, HOLOG_STYLE_D}


#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif // __cplusplus

#endif  //HOLOG_HOLOG_CONF_H
