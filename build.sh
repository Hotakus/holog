#!/bin/bash

# 创建 build 目录
mkdir -p build

(
  cd build || exit
  # 运行 cmake 命令，传递参数
  cmake .. "$@"
  # 使用 make -j 编译
  make -j12
)

# 检查文件是否存在并复制文件
if [ -f "./bin/holog" ]; then
    cp ./bin/holog ./holog/
    echo "File copied successfully."
fi

mkdir -p ./holog
cp ./lib/libholog.a ./holog
cp ./*.h ./holog
cp ./inc/*.h ./holog

