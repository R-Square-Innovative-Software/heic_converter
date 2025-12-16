#!/bin/bash
#
# build_codecs_debian12.sh
# HEIC/HEIF Converter - Embedded Codec Builder for Debian 12/13
# Version: v1.0
# Author: R Square Innovation Software

set -e  # Exit on error

# Color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Directories
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"
CODECS_DIR="${PROJECT_ROOT}/data/debian12"
BUILD_DIR="${CODECS_DIR}/build"
SRC_DIR="${CODECS_DIR}/src"

# Versions - Using versions known to work
X265_VERSION="3.5"  # Known working version
LIBHEIF_VERSION="1.15.2"  # Version with configure script

# URLs
X265_URL="https://bitbucket.org/multicoreware/x265_git/downloads/x265_${X265_VERSION}.tar.gz"
LIBHEIF_URL="https://github.com/strukturag/libheif/releases/download/v${LIBHEIF_VERSION}/libheif-${LIBHEIF_VERSION}.tar.gz"

# Function definitions
fn_print_info() {  ## Local Function
    echo -e "${GREEN}[INFO]${NC} $1"
}  ## End Function fn_print_info

fn_print_warning() {  ## Local Function
    echo -e "${YELLOW}[WARNING]${NC} $1"
}  ## End Function fn_print_warning

fn_print_error() {  ## Local Function
    echo -e "${RED}[ERROR]${NC} $1"
}  ## End Function fn_print_error

fn_check_basic_deps() {  ## Local Function
    fn_print_info "Checking basic build dependencies..."
    
    # Check for essential tools only
    local vs_essential_tools=("cmake" "g++" "make" "wget" "tar")
    
    for s_tool in "${vs_essential_tools[@]}"; do
        if ! command -v "${s_tool}" &> /dev/null; then
            fn_print_error "Essential tool not found: ${s_tool}"
            fn_print_info "Please run: ./scripts/install_deps_debian12.sh"
            exit 1
        fi  ## End if
    done  ## End for
    
    # Check for optional tools (warn if missing)
    if ! command -v "nasm" &> /dev/null; then
        fn_print_warning "nasm not found (optional for x265 optimization)"
    fi  ## End if
    
    if ! command -v "yasm" &> /dev/null; then
        fn_print_warning "yasm not found (optional for x265 optimization)"
    fi  ## End if
    
    fn_print_info "Basic dependencies check passed."
}  ## End Function fn_check_basic_deps

fn_create_directories() {  ## Local Function
    fn_print_info "Creating codec directories..."
    
    mkdir -p "${CODECS_DIR}"
    mkdir -p "${BUILD_DIR}"
    mkdir -p "${SRC_DIR}"
    mkdir -p "${CODECS_DIR}/x265"
    mkdir -p "${CODECS_DIR}/libheif"
    
    fn_print_info "Directories created in: ${CODECS_DIR}"
}  ## End Function fn_create_directories

fn_download_and_extract() {  ## Local Function
    local s_url="$1"
    local s_filename="$2"
    local s_output_dir="$3"
    
    fn_print_info "Processing: ${s_filename}"
    
    # Download
    if [ ! -f "${s_output_dir}/${s_filename}" ]; then
        fn_print_info "Downloading..."
        wget -q --show-progress -O "${s_output_dir}/${s_filename}" "${s_url}"
        if [ $? -ne 0 ]; then
            fn_print_error "Download failed"
            return 1
        fi  ## End if
    else
        fn_print_info "Already downloaded, using cached file."
    fi  ## End if
    
    # Extract
    fn_print_info "Extracting..."
    tar -xzf "${s_output_dir}/${s_filename}" -C "${SRC_DIR}"
    
    return 0
}  ## End Function fn_download_and_extract

fn_build_x265_simple() {  ## Local Function
    fn_print_info "=== Building x265 (HEVC codec) ==="
    
    # Download and extract
    fn_download_and_extract "${X265_URL}" "x265_${X265_VERSION}.tar.gz" "${SRC_DIR}"
    
    # Find the extracted directory
    local s_x265_src=$(find "${SRC_DIR}" -type d -name "*x265*" | head -1)
    if [ -z "$s_x265_src" ]; then
        fn_print_error "Could not find x265 source directory"
        return 1
    fi  ## End if
    
    fn_print_info "Found x265 source at: ${s_x265_src}"
    
    # Create build directory
    local s_x265_build="${BUILD_DIR}/x265"
    mkdir -p "${s_x265_build}"
    
    # Build x265
    cd "${s_x265_src}/source"
    
    fn_print_info "Configuring x265..."
    cmake -G "Unix Makefiles" \
        -DCMAKE_BUILD_TYPE=Release \
        -DENABLE_SHARED=OFF \
        -DENABLE_CLI=OFF \
        -DENABLE_PIC=ON \
        -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
        -B "${s_x265_build}" \
        .
    
    fn_print_info "Building x265..."
    cmake --build "${s_x265_build}" --config Release -j$(nproc)
    
    # Copy the built library
    if [ -f "${s_x265_build}/libx265.a" ]; then
        cp "${s_x265_build}/libx265.a" "${CODECS_DIR}/x265/"
        fn_print_info "Copied: libx265.a"
    else
        # Try alternative location
        find "${s_x265_build}" -name "*.a" -type f -exec cp {} "${CODECS_DIR}/x265/" \;
    fi  ## End if
    
    # Copy headers
    if [ -f "${s_x265_src}/source/x265.h" ]; then
        cp "${s_x265_src}/source/x265.h" "${CODECS_DIR}/x265/"
        fn_print_info "Copied: x265.h"
    fi  ## End if
    
    # Check if x265_config.h exists in build directory
    if [ -f "${s_x265_build}/x265_config.h" ]; then
        cp "${s_x265_build}/x265_config.h" "${CODECS_DIR}/x265/"
        fn_print_info "Copied: x265_config.h"
    fi  ## End if
    
    fn_print_info "x265 build completed."
    return 0
}  ## End Function fn_build_x265_simple

fn_build_libheif_simple() {  ## Local Function
    fn_print_info "=== Building libheif (HEIC/HEIF codec) ==="
    
    # Check if x265 is built
    if [ ! -f "${CODECS_DIR}/x265/libx265.a" ]; then
        fn_print_error "x265 not found. Please build x265 first."
        return 1
    fi  ## End if
    
    # Download and extract
    fn_download_and_extract "${LIBHEIF_URL}" "libheif-${LIBHEIF_VERSION}.tar.gz" "${SRC_DIR}"
    
    # Find the extracted directory
    local s_libheif_src=$(find "${SRC_DIR}" -type d -name "*libheif*" | head -1)
    if [ -z "$s_libheif_src" ]; then
        fn_print_error "Could not find libheif source directory"
        return 1
    fi  ## End if
    
    fn_print_info "Found libheif source at: ${s_libheif_src}"
    
    # Check for configure script
    if [ ! -f "${s_libheif_src}/configure" ]; then
        fn_print_info "Configure script not found. Checking for autogen.sh..."
        
        if [ -f "${s_libheif_src}/autogen.sh" ]; then
            cd "${s_libheif_src}"
            fn_print_info "Running autogen.sh..."
            ./autogen.sh
            if [ $? -ne 0 ]; then
                fn_print_error "autogen.sh failed"
                return 1
            fi  ## End if
        else
            fn_print_error "No configure script or autogen.sh found"
            return 1
        fi  ## End if
    fi  ## End if
    
    # Create build directory
    local s_libheif_build="${BUILD_DIR}/libheif"
    mkdir -p "${s_libheif_build}"
    
    # Build libheif
    cd "${s_libheif_src}"
    
    fn_print_info "Configuring libheif..."
    ./configure \
        --prefix="${s_libheif_build}" \
        --disable-shared \
        --enable-static \
        --disable-examples \
        --disable-go \
        --with-libde265=no \
        --with-x265=yes \
        CPPFLAGS="-I${CODECS_DIR}/x265" \
        LDFLAGS="-L${CODECS_DIR}/x265 -lx265 -lm -lstdc++"
    
    fn_print_info "Building libheif..."
    make -j$(nproc)
    
    fn_print_info "Installing libheif..."
    make install
    
    # Copy the built library
    if [ -f "${s_libheif_build}/lib/libheif.a" ]; then
        cp "${s_libheif_build}/lib/libheif.a" "${CODECS_DIR}/libheif/"
        fn_print_info "Copied: libheif.a"
    fi  ## End if
    
    # Copy headers
    if [ -d "${s_libheif_build}/include" ]; then
        cp -r "${s_libheif_build}/include" "${CODECS_DIR}/libheif/"
        fn_print_info "Copied headers"
    fi  ## End if
    
    fn_print_info "libheif build completed."
    return 0
}  ## End Function fn_build_libheif_simple

fn_create_simple_cmake_config() {  ## Local Function
    fn_print_info "Creating CMake configuration..."
    
    cat > "${CODECS_DIR}/heif_embedded.cmake" << 'EOF'
# heif_embedded.cmake - Embedded HEIC/HEIF codecs for Debian 12/13

# Add include directories
include_directories(${CMAKE_CURRENT_LIST_DIR}/x265)
include_directories(${CMAKE_CURRENT_LIST_DIR}/libheif/include)

# Add library directories  
link_directories(${CMAKE_CURRENT_LIST_DIR}/x265)
link_directories(${CMAKE_CURRENT_LIST_DIR}/libheif)

# Set library paths
set(EMBEDDED_X265_LIB ${CMAKE_CURRENT_LIST_DIR}/x265/libx265.a)
set(EMBEDDED_LIBHEIF_LIB ${CMAKE_CURRENT_LIST_DIR}/libheif/libheif.a)

# Add definitions
add_definitions(-DHAVE_EMBEDDED_HEIC_CODECS)
add_definitions(-DHAVE_X265)
add_definitions(-DHAVE_LIBHEIF)
EOF
    
    fn_print_info "CMake configuration created."
}  ## End Function fn_create_simple_cmake_config

fn_verify_files() {  ## Local Function
    fn_print_info "Verifying built files..."
    
    local b_all_ok=true
    
    # Check x265
    if [ -f "${CODECS_DIR}/x265/libx265.a" ]; then
        fn_print_info "✓ x265 library: OK"
    else
        fn_print_error "✗ x265 library: MISSING"
        b_all_ok=false
    fi  ## End if
    
    if [ -f "${CODECS_DIR}/x265/x265.h" ]; then
        fn_print_info "✓ x265 header: OK"
    else
        fn_print_warning "⚠ x265 header: Missing (may cause build issues)"
    fi  ## End if
    
    # Check libheif
    if [ -f "${CODECS_DIR}/libheif/libheif.a" ]; then
        fn_print_info "✓ libheif library: OK"
    else
        fn_print_error "✗ libheif library: MISSING"
        b_all_ok=false
    fi  ## End if
    
    if [ -d "${CODECS_DIR}/libheif/include" ]; then
        fn_print_info "✓ libheif headers: OK"
    else
        fn_print_warning "⚠ libheif headers: Missing (may cause build issues)"
    fi  ## End if
    
    if [ "$b_all_ok" = true ]; then
        fn_print_info "All essential files are present."
        return 0
    else
        fn_print_error "Some essential files are missing."
        return 1
    fi  ## End if
}  ## End Function fn_verify_files

fn_main() {  ## Local Function
    echo "========================================"
    echo "Building Embedded HEIC/HEIF Codecs"
    echo "For Debian 12/13"
    echo "Version: v1.0"
    echo "========================================"
    echo ""
    
    # Check basic dependencies
    fn_check_basic_deps
    
    # Create directories
    fn_create_directories
    
    # Build x265
    echo ""
    echo "=== STEP 1: Building x265 ==="
    if fn_build_x265_simple; then
        echo "x265 build: SUCCESS"
    else
        echo "x265 build: FAILED"
        exit 1
    fi  ## End if
    
    # Build libheif
    echo ""
    echo "=== STEP 2: Building libheif ==="
    if fn_build_libheif_simple; then
        echo "libheif build: SUCCESS"
    else
        echo "libheif build: FAILED"
        exit 1
    fi  ## End if
    
    # Create CMake config
    fn_create_simple_cmake_config
    
    # Verify files
    fn_verify_files
    
    echo ""
    echo "========================================"
    echo "BUILD SUMMARY"
    echo "========================================"
    echo "Built components:"
    echo "  • x265 ${X265_VERSION} (HEVC codec)"
    echo "  • libheif ${LIBHEIF_VERSION} (HEIC/HEIF codec)"
    echo ""
    echo "Output directory: ${CODECS_DIR}"
    echo ""
    echo "Files created:"
    ls -la "${CODECS_DIR}/x265/" 2>/dev/null || echo "x265 directory empty"
    ls -la "${CODECS_DIR}/libheif/" 2>/dev/null || echo "libheif directory empty"
    echo ""
    echo "Next step: Build the converter"
    echo "  ./scripts/build_debian12.sh"
    echo "========================================"
}  ## End Function fn_main

# Run main function
fn_main

exit 0