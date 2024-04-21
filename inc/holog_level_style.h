/**
  ******************************************************************************
  * @file           : holog_level_style.h
  * @author         : Hotakus (hotakus@foxmail.com)
  * @brief          : None
  * @date           : 2024/4/21
  ******************************************************************************
  */

#ifndef HOLOG_HOLOG_LEVEL_STYLE_H
#define HOLOG_HOLOG_LEVEL_STYLE_H

typedef struct holog_log_level_t {
    const char *level;
    const char *color;
} holog_log_level_t;

#define HOLOG_LOG_LEVEL_STYLE_INFO          {"INF", HOLOG_COLOR_LIGHT_CYAN}
#define HOLOG_LOG_LEVEL_STYLE_ERROR         {"ERR", HOLOG_COLOR_RED}
#define HOLOG_LOG_LEVEL_STYLE_WARNING       {"WAR", HOLOG_COLOR_YELLOW}
#define HOLOG_LOG_LEVEL_STYLE_FATAL         {"FAT", HOLOG_COLOR_LIGHT_RED}
#define HOLOG_LOG_LEVEL_STYLE_DEBUG         {"DEB", HOLOG_COLOR_DARY_GRAY}
#define HOLOG_LOG_LEVEL_STYLE_TRACE         {"TRA", HOLOG_COLOR_BROWN}
#define HOLOG_LOG_LEVEL_STYLE_DEFAULT       {HOLOG_LOG_LEVEL_STYLE_INFO  , HOLOG_LOG_LEVEL_STYLE_ERROR, \
                                            HOLOG_LOG_LEVEL_STYLE_WARNING, HOLOG_LOG_LEVEL_STYLE_FATAL, \
                                            HOLOG_LOG_LEVEL_STYLE_DEBUG  , HOLOG_LOG_LEVEL_STYLE_TRACE}

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif // __cplusplus

#endif  //HOLOG_HOLOG_LEVEL_STYLE_H
