#!/bin/bash

# Install requirements
pip install -r scripts/requirements.txt

# Create build directory
mkdir -p build
cd ./build

# Configure CMake build
cmake ../draco -DDRACO_TRANSCODER_SUPPORTED=ON -DCMAKE_BUILD_TYPE=Release

# Build with CMake
cmake --build . -- -j4

# Create links to executables
ln -sf ./build/draco_encoder ../encoder
ln -sf ./build/draco_decoder ../decoder
