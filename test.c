/**
  ******************************************************************************
  * @file           : test.c
  * @author         : Hotakus (hotakus@foxmail.com)
  * @brief          : None
  * @date           : 2024/4/19
  ******************************************************************************
  */

#include <stdio.h>
#include <unistd.h>
#include "holog.h"


int main() {
    // HoLog initialize
    holog_init();

    // you can create log devices very easily, and flexibly
    // Log devices can be set to different log levels very flexibly, if you don't set the log level, it will ignore corresponding level logs.
    // Also support embedded filesystem for embedded system
    holog_device_t *stdout_dev = holog()->create("stdout_dev", HOLOG_DEVICE_TYPE_STDOUT, HOLOG_LEVEL_ERROR | HOLOG_LEVEL_WARNING | HOLOG_LEVEL_INFO, HOLOG_LINEFEED_CRLF);
    holog_device_t *info_dev = holog()->create("info_dev", HOLOG_DEVICE_TYPE_COMMON_FILE, HOLOG_LEVEL_INFO, HOLOG_LINEFEED_LF);
    holog_device_t *warning_dev = holog()->create("warning_dev", HOLOG_DEVICE_TYPE_COMMON_FILE, HOLOG_LEVEL_WARNING, HOLOG_LINEFEED_LF);
    holog_device_t *error_dev = holog()->create("error_dev", HOLOG_DEVICE_TYPE_COMMON_FILE, HOLOG_LEVEL_ERROR, HOLOG_LINEFEED_LF);
    holog_device_t *mix_dev = holog()->create("mix_dev", HOLOG_DEVICE_TYPE_COMMON_FILE, HOLOG_LEVEL_ALL, HOLOG_LINEFEED_LF);

    /* FatFs device example */
    // holog_device_t *fatfs_dev = holog()->create("fatfs_dev", HOLOG_DEVICE_TYPE_FATFS, HOLOG_LEVEL_ALL, HOLOG_LINEFEED_LF);
    // holog()->set_log_path(fatfs_dev, "0:/all.log"); /* FatFs device, log info to all.log, the "0:/" is your fatfs mount point */
    // holog()->register_device(fatfs_dev);

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
    holog_warning("This is warning 0x%x.", 0xdeadbeef);
    holog_error("This is %s.", "error.");
    holog_trace("This is trace.");
    holog_debug("This is debug.");
    holog_fatal("This is fatal.");

    holog_deinit();
    return 0;
}
