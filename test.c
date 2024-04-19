/**
  ******************************************************************************
  * @file           : test.c
  * @author         : Hotakus (hotakus@foxmail.com)
  * @brief          : None
  * @date           : 2024/4/19
  ******************************************************************************
  */

#include <stdio.h>
#include <stdarg.h>
#include "holog.h"

void test_print(int num_args, ...) {
    // 创建一个 va_list 对象
    va_list args;

    // 使用 va_start 宏初始化 va_list
    va_start(args, num_args);

    // 遍历参数并打印它们的内容
    for (int i = 0; i < num_args - 1; i++) {
        // 使用 va_arg 宏提取参数的内容
        char *arg = va_arg(args, char *);
        printf("Argument %d: %s\n", i + 1, arg);
    }

    // 使用 va_end 宏结束可变参数列表的处理
    va_end(args);
}

int main() {
    printf("test\n");
    holog_init();

    holog_device_t *dev = holog()->create("test_dev", HOLOG_DEVICE_TYPE_STDOUT, HOLOG_LEVEL_INFO | HOLOG_LEVEL_WARNING);
    holog()->set_log_path(dev, "./test.log");
    holog()->register_device(dev);

    holog_info("test2 fsdsf s");

//    test_print(2, __FILE__, __FILE_NAME__, __LINE__);

    return 0;
}
