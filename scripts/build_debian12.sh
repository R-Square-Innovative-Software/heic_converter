#!/bin/bash
# HEIC/HEIF Converter - Build Script for Debian 12/13
# Author: R Square Innovation Software
# Version: v1.0

set -e

echo "========================================"
echo "Building HEIC Converter for Debian 12/13"
echo "========================================"
echo ""

# Check if dependencies are installed
if ! dpkg -l | grep -q "libpng-dev"; then
    echo "Installing dependencies..."
    ./scripts/install_deps_debian12.sh
fi

# Check if codecs are built
CODECS_DIR="data/debian12"
if [ ! -f "${CODECS_DIR}/x265/libx265.a" ] || [ ! -f "${CODECS_DIR}/libheif/libheif.a" ]; then
    echo "Embedded codecs not found. Building them..."
    bash ./scripts/build_codecs_debian12.sh
fi

# Create build directory
BUILD_DIR="build/debian12"
mkdir -p ${BUILD_DIR}
cd ${BUILD_DIR}

# Configure with CMake
echo "Configuring build..."
cmake ../.. -DDEBIAN12_BUILD=ON -DCMAKE_BUILD_TYPE=Release

# Build
echo "Building..."
make -j$(nproc)

# Check if build was successful
if [ -f "bin/heic_converter" ]; then
    echo ""
    echo "========================================"
    echo "Build successful!"
    echo "Binary location: ${BUILD_DIR}/bin/heic_converter"
    echo "========================================"
    
    # Optional: Run a quick test
    echo ""
    echo "Running basic test..."
    ./bin/heic_converter --version
else
    echo "Build failed!"
    exit 1
fi

cd ../..