#!/bin/bash

BUILD_DIR="./build"

if ! [[ -d "$BUILD_DIR" ]]; then
    mkdir $BUILD_DIR
fi

echo "Started compiling..."

set -e  # Exit on error

g++ -c -o $BUILD_DIR/libs.o libs.cpp
ar cr $BUILD_DIR/libs.a $BUILD_DIR/libs.o

echo "Compiled static library ($BUILD_DIR/libs.a)"

g++ -c -o $BUILD_DIR/task9s.o task9s.cpp
g++ -static -o task9s $BUILD_DIR/task9s.o -L $BUILD_DIR $BUILD_DIR/libs.a

echo "Compiled executable with static library (./task9s)"

g++ -c -fPIC -o $BUILD_DIR/libd.o libd.cpp
g++ -shared -fPIC -o $BUILD_DIR/libd.so $BUILD_DIR/libd.o

echo "Compiled dynamic library ($BUILD_DIR/libd.so)"

g++ -c -o $BUILD_DIR/task9d.o task9d.cpp
g++ -o task9d $BUILD_DIR/task9d.o -L $BUILD_DIR $BUILD_DIR/libd.so

echo "Compiled executable with dynamic library (./task9d)"

g++ -c -o $BUILD_DIR/task9dr.o task9dr.cpp
g++ -o task9dr $BUILD_DIR/task9dr.o -L $BUILD_DIR $BUILD_DIR/libd.so

set +e

echo "Compiled executable with dynamic library by request (./task9dr)"
echo "Don't forget to set LD_LIBRARY_PATH=$BUILD_DIR"

echo "Done!"