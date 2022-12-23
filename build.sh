#!/bin/bash

BUILD_DIR=../zotero_to_file_tree_build
SOURCE_DIR=./
INSTALL_DIR=../zotero_to_file_tree_install

mkdir $BUILD_DIR 

cmake -B $BUILD_DIR -S $SOURCE_DIR -G Ninja --preset "unixlike-clang-release" \
-DBUILD_SHARED_LIBS=OFF \
-DCMAKE_BUILD_TYPE=Release \
-DCMAKE_CXX_STANDARD=20 \
-Dzotero_to_file_tree_WARNINGS_AS_ERRORS=ON \

threads=`nproc`
cmake --build $BUILD_DIR  --config Release --parallel --parallel $threads

mkdir $INSTALL_DIR
cmake --install ./$BUILD_DIR --config Release --prefix $INSTALL_DIR
