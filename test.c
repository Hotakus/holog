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

    // HoLog initialize
    holog_init();

    // you can create log devices very easily, and flexibly
    // Log devices can be set to different log levels very flexibly, if you don't set the log level, it will ignore corresponding level logs.
//    // Also support embedded filesystem for embedded system
    holog_device_t *stdout_dev = holog()->create("stdout_dev", HOLOG_DEVICE_TYPE_STDOUT, HOLOG_LEVEL_ERROR | HOLOG_LEVEL_WARNING | HOLOG_LEVEL_INFO, true);
    holog_device_t *info_dev = holog()->create("info_dev", HOLOG_DEVICE_TYPE_COMMON_FILE, HOLOG_LEVEL_INFO, false);
    holog_device_t *warning_dev = holog()->create("warning_dev", HOLOG_DEVICE_TYPE_COMMON_FILE, HOLOG_LEVEL_WARNING, false);
    holog_device_t *error_dev = holog()->create("error_dev", HOLOG_DEVICE_TYPE_COMMON_FILE, HOLOG_LEVEL_ERROR, false);
    holog_device_t *mix_dev = holog()->create("mix_dev", HOLOG_DEVICE_TYPE_COMMON_FILE, HOLOG_LEVEL_ERROR | HOLOG_LEVEL_WARNING | HOLOG_LEVEL_INFO, false);

    // Set log path
    holog()->set_log_path(stdout_dev, NULL);    // stdout don't need log path, set to NULL
    holog()->set_log_path(info_dev, "./info.log");
    holog()->set_log_path(warning_dev, "./warning.log");
    holog()->set_log_path(error_dev, "./error.log");
    holog()->set_log_path(mix_dev, "./all.log");

    // Register devices
    holog()->register_device(stdout_dev);
    holog()->register_device(info_dev);
    holog()->register_device(warning_dev);
    holog()->register_device(error_dev);
    holog()->register_device(mix_dev);

    // Print logs
    // Programe will assign logs automatically to corresponding devices you registered
    holog_info("This is info %d.", 114514);
    holog_warning("This is warning.");
    holog_error("This is error.");

//    holog_device_t *stdout_dev = holog()->create("stdout_dev", HOLOG_DEVICE_TYPE_STDOUT, HOLOG_LEVEL_ALL);
//    holog()->register_device(stdout_dev);
//
//    holog_debug("This is debug ");
//    holog_trace("This is trace ");
//    holog_info("This is info");
//    holog_error(HOLOG_COLOR_RED"This is error %d"HOLOG_COLOR_NONE, 114514);
//    holog_warning("This is warning");
//    holog_fatal("This is fatal");

    return 0;
}
