#!/bin/bash

# Install requirements
pip install -r scripts/requirements.txt

# Create build directory
mkdir -p build
cd ./build

cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -- -j10

# Create links to executables
ln -sf ./build/bin/encoder ../encoder
ln -sf ./build/bin/decoder ../decoder
