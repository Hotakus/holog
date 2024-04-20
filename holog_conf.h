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

#define HOLOG_BANNER "[Recorded by HoLog]"

// the size is for log buffer, min is 256
#define HOLOG_PRINTF_MAX_SIZE       (256)
#define HOLOG_LINEFEED              "\n"

// enable or disable
#define HOLOG_STDOUT_ENABLED        (1)
#define HOLOG_COMMON_FILE_ENABLED   (1)
#define HOLOG_FATFS_ENABLED         (0)
#define HOLOG_LITTLEFS_ENABLED      (0)

#if (HOLOG_FATFS_ENABLED == 1)
#include "ff.h"
#endif
#if (HOLOG_LITTLEFS_ENABLED == 1)
#include "littlefs.h"
#endif

// customize your own timestamp
#define HOLOG_GET_TIMESTAMP()    (time(NULL))

// color
#define HOLOG_USE_COLOR          (1)
#define HOLOG_COLOR_NONE         "\033[m"
#define HOLOG_COLOR_RED          "\033[0;32;31m"
#define HOLOG_COLOR_LIGHT_RED    "\033[1;31m"
#define HOLOG_COLOR_GREEN        "\033[0;32;32m"
#define HOLOG_COLOR_LIGHT_GREEN  "\033[1;32m"
#define HOLOG_COLOR_BLUE         "\033[0;32;34m"
#define HOLOG_COLOR_LIGHT_BLUE   "\033[1;34m"
#define HOLOG_COLOR_DARY_GRAY    "\033[1;30m"
#define HOLOG_COLOR_CYAN         "\033[0;36m"
#define HOLOG_COLOR_LIGHT_CYAN   "\033[1;36m"
#define HOLOG_COLOR_PURPLE       "\033[0;35m"
#define HOLOG_COLOR_LIGHT_PURPLE "\033[1;35m"
#define HOLOG_COLOR_BROWN        "\033[0;33m"
#define HOLOG_COLOR_YELLOW       "\033[1;33m"
#define HOLOG_COLOR_LIGHT_GRAY   "\033[0;37m"
#define HOLOG_COLOR_WHITE        "\033[1;37m"

// you can customize color for each part of style
#define HOLOG_LOG_STYLE_LIST_INFO    {"INFO   ", HOLOG_COLOR_CYAN}
#define HOLOG_LOG_STYLE_LIST_ERROR   {"ERROR  ", HOLOG_COLOR_RED}
#define HOLOG_LOG_STYLE_LIST_WARNING {"WARNING", HOLOG_COLOR_YELLOW}
#define HOLOG_LOG_STYLE_LIST_FATAL   {"FATAL  ", HOLOG_COLOR_LIGHT_RED}
#define HOLOG_LOG_STYLE_LIST_DEBUG   {"DEBUG  ", HOLOG_COLOR_DARY_GRAY}
#define HOLOG_LOG_STYLE_LIST_TRACE   {"TRACE  ", HOLOG_COLOR_BROWN}

// holog style
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

// customize your own style, the ABCD is the order of style item.
// the HOLOG_STYLE_MAIN_CONTENT is the main body of log.
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
