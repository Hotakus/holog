#!/bin/bash

# 创建 build 目录
mkdir -p build
cd build
# 运行 cmake 命令，传递参数
cmake .. "$@"
# 使用 make -j 编译
make -j12
cd ..
cp ./bin/holog .

mkdir -p ./archive

cp ./lib/libholog.a ./archive
cp ./*.h ./archive

