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


#ifdef __cplusplus
extern "C" {
#endif
typedef enum holog_res_t {
    HOLOG_RES_OK = 0,
    HOLOG_RES_ERROR
} holog_res_t;


typedef struct holog_t {
    struct {
//        holog_res_t (*)
    };
} holog_t;

holog_t *holog_create();
holog_res_t holog_destroy(holog_t *holog);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif  //HOLOG_HOLOG_H
