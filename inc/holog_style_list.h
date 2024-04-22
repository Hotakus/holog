/**
  ******************************************************************************
  * @file           : holog_style_list.h
  * @author         : Hotakus (hotakus@foxmail.com)
  * @brief          : None
  * @date           : 2024/4/21
  ******************************************************************************
  */

#ifndef HOLOG_HOLOG_STYLE_LIST_H
#define HOLOG_HOLOG_STYLE_LIST_H

typedef struct holog_style_list_t {
    holog_style_define_t style;
    const char *bracket;
} holog_style_list_t;

#define HOLOG_CUSTOM_STYLE_1_A              {HOLOG_STYLE_TIME,          "[]"}
#define HOLOG_CUSTOM_STYLE_1_B              {HOLOG_STYLE_LEVEL,         "[]"}
#define HOLOG_CUSTOM_STYLE_1_C              {HOLOG_STYLE_FILE_NAME,     "[]"}
#define HOLOG_CUSTOM_STYLE_1_D              {HOLOG_STYLE_MAIN_CONTENT,  "  "}
#define HOLOG_CUSTOM_STYLE_DEFAULT          {HOLOG_CUSTOM_STYLE_1_A, HOLOG_CUSTOM_STYLE_1_B, \
                                            HOLOG_CUSTOM_STYLE_1_C, HOLOG_CUSTOM_STYLE_1_D}

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif // __cplusplus

#endif  //HOLOG_HOLOG_STYLE_LIST_H
