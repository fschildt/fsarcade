#!/bin/sh

mkdir -p build
cd build

if [[ "$0" == "Release" ]]; then
    echo "Release"
    cmake -DCMAKE_BUILD_TYPE=Release ../cmake
else
    echo "Debug"
    cmake -DCMAKE_BUILD_TYPE=Debug ../cmake
fi

make
cd ..
