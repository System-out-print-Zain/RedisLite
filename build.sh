#!/bin/bash

BUILD_TYPE=${1:-Debug}

if [[ "$BUILD_TYPE" != "Debug" && "$BUILD_TYPE" != "Release" ]]; then
    echo "Error: Build type must be either Debug or Release"
    echo "Usage: $0 [Debug|Release]"
    exit 1
fi

set -e

mkdir -p build

cd build

echo "Building in $BUILD_TYPE mode..."
cmake .. -DCMAKE_BUILD_TYPE="$BUILD_TYPE"

cmake --build .

cd ..

GREEN='\033[0;32m'
NC='\033[0m'
echo "${GREEN}Build completed successfully!${NC}"