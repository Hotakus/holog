/**
  ******************************************************************************
  * @file           : holog_utils.h
  * @author         : Hotakus (hotakus@foxmail.com)
  * @brief          : None
  * @date           : 2024/4/19
  ******************************************************************************
  */

#ifndef HOLOG_HOLOG_UTILS_H
#define HOLOG_HOLOG_UTILS_H


#include <stdint.h>

__attribute__((weak)) char * timestamp_to_time_string(size_t timestamp);
__attribute__((weak)) char * timestamp_to_filename(size_t timestamp);

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif // __cplusplus

#endif  //HOLOG_HOLOG_UTILS_H
