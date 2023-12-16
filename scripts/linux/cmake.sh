#!/bin/bash

SCRIPT_PATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"

MADARA_PATH=$SCRIPT_PATH/../..

echo SCRIPT_PATH=$SCRIPT_PATH
echo MADARA_PATH=$MADARA_PATH

cd $MADARA_PATH
mkdir build
mkdir install
cd build
cmake -DCMAKE_INSTALL_PREFIX="..\install"  -D'CMAKE_INSTALL_PREFIX=/usr/local' -DCMAKE_PREFIX_PATH=$MADARA_PATH/install ..
cmake --build .  --config debug -j$(nproc)
cmake --build .  --config release
sudo cmake --build .  --target install --config release
sudo cmake --build .  --target install --config debug