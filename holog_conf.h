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

#define HOLOG_BANNER                "[Recorded by HoLog]"

#define HOLOG_LINEFEED_CRLF         "\r\n"
#define HOLOG_LINEFEED_LF           "\n"

// the size is for log buffer
// the max of main content is (HOLOG_PRINTF_MAX_SIZE - 128)
#define HOLOG_PRINTF_MAX_SIZE       (256)

// Methods of logging, enable is 1, disable is 0
#define HOLOG_STDOUT_ENABLED        (1)     // stdout
#define HOLOG_COMMON_FILE_ENABLED   (0)     // common file
#define HOLOG_FATFS_ENABLED         (0)     // fatfs
#define HOLOG_LITTLEFS_ENABLED      (0)     // littlefs

// include files
#if (HOLOG_FATFS_ENABLED == 1)
#include "ff.h"
#endif
#if (HOLOG_LITTLEFS_ENABLED == 1)
#include "lfs.h"
#endif

// if your platform is not linux or windows, such as arm, you can customize your own timestamp
#if defined(__linux__) || defined(_WIN32)
#define HOLOG_GET_TIMESTAMP()    (time(NULL))   // don't modify
#else
#define HOLOG_GET_TIMESTAMP()    (0)            // customize your own timestamp
#endif

// auto create log file by date that generated by HOLOG_GET_TIMESTAMP()
// if enable it, the log path of device is a directory, such as 0:/
#define HOLOG_AUTO_FILE_CREATE_BY_DATE    (0)
#define HOLOG_AUTO_FILE_FORMAT            "%04d-%02d-%02d.log"

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
#include "./inc/holog_level_style.h"
#define HOLOG_LOG_LEVEL_STYLE_1_INFO       {"I", HOLOG_COLOR_LIGHT_CYAN}
#define HOLOG_LOG_LEVEL_STYLE_1_ERROR      {"E", HOLOG_COLOR_RED}
#define HOLOG_LOG_LEVEL_STYLE_1_WARNING    {"W", HOLOG_COLOR_YELLOW}
#define HOLOG_LOG_LEVEL_STYLE_1_FATAL      {"F", HOLOG_COLOR_LIGHT_RED}
#define HOLOG_LOG_LEVEL_STYLE_1_DEBUG      {"D", HOLOG_COLOR_DARY_GRAY}
#define HOLOG_LOG_LEVEL_STYLE_1_TRACE      {"T", HOLOG_COLOR_BROWN}
#define HOLOG_LOG_LEVEL_STYLE_1            {HOLOG_LOG_LEVEL_STYLE_1_INFO   , HOLOG_LOG_LEVEL_STYLE_1_ERROR, \
                                            HOLOG_LOG_LEVEL_STYLE_1_WARNING, HOLOG_LOG_LEVEL_STYLE_1_FATAL, \
                                            HOLOG_LOG_LEVEL_STYLE_1_DEBUG  , HOLOG_LOG_LEVEL_STYLE_1_TRACE}

// customize your own style for each log level, or use ./inc/holog_level_style.h
#define HOLOG_LOG_LEVEL_STYLE              HOLOG_LOG_LEVEL_STYLE_1

// holog style
typedef enum holog_style_define_t {
    HOLOG_STYLE_NONE,
    HOLOG_STYLE_LEVEL,
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
// example: <HOLOG_STYLE_TIME> <HOLOG_STYLE_LEVEL> <HOLOG_STYLE_FILE_NAME> <HOLOG_STYLE_MAIN_CONTENT>
// out: [16:01:20 INFO] holog_conf.h:100: your information.
#include "./inc/holog_style_list.h"
#define HOLOG_DEFAULT_STYLE_A          {HOLOG_STYLE_TIME,          "[]"}
#define HOLOG_DEFAULT_STYLE_B          {HOLOG_STYLE_LEVEL,         "[]"}
#define HOLOG_DEFAULT_STYLE_C          {HOLOG_STYLE_FILE_NAME,     " :"}
#define HOLOG_DEFAULT_STYLE_D          {HOLOG_STYLE_MAIN_CONTENT,  "  "}
#define HOLOG_STYLE_DEFAULT            {HOLOG_DEFAULT_STYLE_A, HOLOG_DEFAULT_STYLE_B, \
                                        HOLOG_DEFAULT_STYLE_C, HOLOG_DEFAULT_STYLE_D}

// customize your own log style, or use ./inc/holog_style_list.h
#define HOLOG_LOG_STYLE_LIST            HOLOG_STYLE_DEFAULT

// extra define
#ifndef __FILE_NAME__
#define __FILE_NAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

#define HOLOG_USE_CUSTOM_MALLOC   (0)
#if (HOLOG_USE_CUSTOM_MALLOC == 1)      // Custom malloc
#define HOLOG_MALLOC(size)              malloc(size)
#define HOLOG_FREE(ptr)                 free(ptr)
#define HOLOG_CALLOC(count, size)       calloc(count, size)
#define HOLOG_REALLOC(ptr, size)        realloc(ptr, size)
#else                                   // default malloc
#include <stdlib.h>
#define HOLOG_MALLOC(size)              malloc(size)
#define HOLOG_FREE(ptr)                 free(ptr)
#define HOLOG_CALLOC(count, size)       calloc(count, size)
#define HOLOG_REALLOC(ptr, size)        realloc(ptr, size)
#endif

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif // __cplusplus

#endif  //HOLOG_HOLOG_CONF_H
