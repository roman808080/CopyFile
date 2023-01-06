#!/bin/env bash

rm -r build
mkdir -p build
source local_python/bin/activate

pushd build

conan install .. --build=missing -pr:b  copy_file_debug_profile -pr:h copy_file_debug_profile
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Debug
make

popd
