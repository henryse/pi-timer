#!/usr/bin/env bash

release_dir=$(pwd)/build/release
debug_dir=$(pwd)/build/debug

if [ ! -d "$release_dir" ]; then
    mkdir -p ${release_dir}
fi

if [ ! -f ${release_dir}/CMakeCache.txt ]; then
    cd ${release_dir}
    cmake -DCMAKE_BUILD_TYPE=Release ../..
fi

if [ ! -d "$debug_dir" ]; then
    mkdir -p ${debug_dir}
fi

if [ ! -f ${debug_dir}/CMakeCache.txt ]; then
    cd ${debug_dir}
    cmake -DCMAKE_BUILD_TYPE=Debug ../..
fi

