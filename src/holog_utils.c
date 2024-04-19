/**
  ******************************************************************************
  * @file           : holog_utils.c
  * @author         : Hotakus (hotakus@foxmail.com)
  * @brief          : None
  * @date           : 2024/4/19
  ******************************************************************************
  */

#include <time.h>
#include <malloc.h>
#include "holog_utils.h"
#include "holog_conf.h"

__attribute__((weak)) char *timestamp_to_time_string(size_t timestamp) {
    struct tm *tm = localtime((const time_t *)HOLOG_GET_TIMESTAMP());
    char *buf = malloc(HOLOG_PRINTF_MAX_SIZE);

    strftime(buf, HOLOG_PRINTF_MAX_SIZE, "%Y-%m-%d %H:%M:%S", tm);

    return NULL;
}

__attribute__((weak)) char *timestamp_to_filename(size_t timestamp) {

}
