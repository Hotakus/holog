cmake_minimum_required(VERSION 3.20)
project(holog
        LANGUAGES C
        VERSION 1.0.0.0
        DESCRIPTION "HoLog"
        HOMEPAGE_URL https://github.com/Hotakus/holog
)
set(CMAKE_C_STANDARD 99)

# 指定版本号的配置文件
configure_file(${PROJECT_SOURCE_DIR}/version.h.in ${PROJECT_SOURCE_DIR}/version.h)

#设置输出目录
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../lib) # 设置静态库文件目录
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../lib) # 动态库文件目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin) # 可执行文件目录

#设置编译、链接参数等
#set(LINK_FLAGS      "${LINK_FLAGS} -Wl,-Map=object.map,--cref,--gc-section")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fdata-sections -ffunction-sections -Wall")

# main add
add_definitions(-DDUMMY_DEFINE)
add_subdirectory(./src/homsg_psp)

if (HOLOG_USE_BUILTIN_FATFS)
    add_subdirectory(./ctp/fatfs)
endif ()

if (HOLOG_USE_BUILTIN_LITTLEFS)
    add_subdirectory(./ctp/littlefs)
endif ()

#if (HOLOG_USE_TEST)
#    add_executable(holog ./src/holog.c test.c)
#else ()
#    add_library(holog STATIC ./src/holog.c)
#endif ()
add_executable(holog ./src/holog.c test.c)

target_link_libraries(holog homsg_psp)
target_include_directories(holog PUBLIC
                           ${CMAKE_CURRENT_SOURCE_DIR}
                           ${CMAKE_CURRENT_SOURCE_DIR}/inc
                           ${CMAKE_CURRENT_SOURCE_DIR}/src/homsg_psp
                           ${CMAKE_CURRENT_SOURCE_DIR}/src/homsg_psp/src/chain

                           ${CMAKE_CURRENT_SOURCE_DIR}/ctp/fatfs/inc
                           ${CMAKE_CURRENT_SOURCE_DIR}/ctp/littlefs
)

if (HOLOG_USE_BUILTIN_FATFS)
    target_link_libraries(holog fatfs)
endif ()

if (HOLOG_USE_BUILTIN_LITTLEFS)
    target_link_libraries(holog lfs)
endif ()