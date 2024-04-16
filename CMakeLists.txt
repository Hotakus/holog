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

# 系统设置
if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    MESSAGE(STATUS "Current platform: Linux ")
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE Debug)
    endif()
elseif (CMAKE_SYSTEM_NAME MATCHES "Windows")
    MESSAGE(STATUS "Current platform: Windows")
elseif (CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
    MESSAGE(STATUS "Current platform: FreeBSD")
else ()
    MESSAGE(STATUS "Unknown platform: ${CMAKE_SYSTEM_NAME}")
endif (CMAKE_SYSTEM_NAME MATCHES "Linux")
MESSAGE(STATUS "Current build type: ${CMAKE_BUILD_TYPE}")


find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    message(STATUS "Set up ccache ...")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
else ()
    message(WARNING "Can not find ccache. (you can ignore this tip.)")
endif()

#设置输出目录
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../lib) # 设置静态库文件目录
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../lib) # 动态库文件目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin) # 可执行文件目录

#设置编译、链接参数等
#set(LINK_FLAGS      "${LINK_FLAGS} -Wl,-Map=object.map,--cref,--gc-section")
#set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -fdata-sections -ffunction-sections -Wall")

# main add
add_definitions(-DDUMMY_DEFINE)
add_compile_options(-fdata-sections -ffunction-sections -Wall)
add_link_options(-Wl,-Map=object.map,--cref,--gc-section)
include_directories(./inc ./)

#file(GLOB_RECURSE SOURCES "./*.c" "./*.h")
#add_subdirectory(./eg)

#message(WARNING     "${LINK_FLAGS}")
#message(WARNING     "${CMAKE_C_FLAGS}")

add_executable(holog holog.c)
#target_link_libraries(holog eg)
