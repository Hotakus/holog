# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_LIB_SRCS 变量
aux_source_directory(./src LIB_SRCS)
include_directories(./inc LIB_INC)
include_directories(. LIB_INC)

# 生成链接库
add_library(fatfs ${LIB_SRCS} ${LIB_INC})
target_include_directories(chain PUBLIC
                           ${CMAKE_CURRENT_SOURCE_DIR}/inc
)