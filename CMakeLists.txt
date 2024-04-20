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
#set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -fdata-sections -ffunction-sections -Wall")

# main add
add_definitions(-DDUMMY_DEFINE)
#add_compile_options(-fdata-sections -ffunction-sections -Wall -O2)
#add_link_options(-Wl,-Map=object.map,--cref,--gc-section)

include_directories(./inc ./)
include_directories(./src/homsg_psp ./src/homsg_psp/src/chain)


#file(GLOB_RECURSE SOURCES "./*.c" "./*.h")
#add_subdirectory(./eg)
add_subdirectory(./src/homsg_psp)


include(${PROJECT_SOURCE_DIR}/toolchain.cmake)

add_executable(holog ./src/holog.c ./src/holog_utils.c ./test.c)
#add_library(holog ./src/holog.c ./src/holog_utils.c)
target_link_libraries(holog homsg_psp)
