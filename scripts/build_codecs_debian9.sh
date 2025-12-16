#!/bin/bash
#
# build_codecs_debian9.sh
# HEIC/HEIF Converter - Embedded Codec Builder for Debian 9
# Version: v1.0
# Author: R Square Innovation Software
#
# This script downloads and builds embedded HEIC/HEIF codecs
# for Debian 9 systems. These codecs are built statically
# and embedded in the application, so no system installation is required.

set -e  # Exit on error

# Color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Directories
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"
CODECS_DIR="${PROJECT_ROOT}/data/debian9"
BUILD_DIR="${CODECS_DIR}/build"
SRC_DIR="${CODECS_DIR}/src"

# Versions (older compatible releases for Debian 9)
X265_VERSION="3.4"  # Compatible with older GCC
LIBHEIF_VERSION="1.12.0"  # Compatible with older dependencies

# URLs
X265_URL="https://bitbucket.org/multicoreware/x265_git/downloads/x265_${X265_VERSION}.tar.gz"
LIBHEIF_URL="https://github.com/strukturag/libheif/releases/download/v${LIBHEIF_VERSION}/libheif-${LIBHEIF_VERSION}.tar.gz"

# Function definitions (same as Debian 12, but with different build flags)

fn_print_info() {  # Local Function
    echo -e "${GREEN}[INFO]${NC} $1"
}  ## End Function fn_print_info

fn_print_warning() {  ## Local Function
    echo -e "${YELLOW}[WARNING]${NC} $1"
}  ## End Function fn_print_warning

fn_print_error() {  ## Local Function
    echo -e "${RED}[ERROR]${NC} $1"
}  ## End Function fn_print_error

fn_check_dependencies() {  ## Local Function
    fn_print_info "Checking build dependencies..."
    
    local vs_required_tools=(
        "cmake"
        "g++"
        "make"
        "wget"
        "tar"
        "pkg-config"
        "nasm"
        "yasm"
    )
    
    local s_tool
    for s_tool in "${vs_required_tools[@]}"; do
        if ! command -v "${s_tool}" &> /dev/null; then
            fn_print_error "Required tool not found: ${s_tool}"
            fn_print_info "Please install it with: sudo apt-get install ${s_tool}"
            exit 1
        fi  ## End if
    done  ## End for
    
    fn_print_info "All build dependencies are available."
}  ## End Function fn_check_dependencies

fn_create_directories() {  ## Local Function
    fn_print_info "Creating codec directories..."
    
    mkdir -p "${CODECS_DIR}"
    mkdir -p "${BUILD_DIR}"
    mkdir -p "${SRC_DIR}"
    mkdir -p "${BUILD_DIR}/x265"
    mkdir -p "${BUILD_DIR}/libheif"
    
    fn_print_info "Directories created in: ${CODECS_DIR}"
}  ## End Function fn_create_directories

fn_download_source() {  ## Local Function
    local s_url="$1"
    local s_filename="$2"
    local s_output_dir="$3"
    
    fn_print_info "Downloading: ${s_filename}"
    
    if [ -f "${s_output_dir}/${s_filename}" ]; then
        fn_print_info "File already exists, skipping download."
        return 0
    fi  ## End if
    
    wget -q --show-progress -O "${s_output_dir}/${s_filename}" "${s_url}"
    
    if [ $? -ne 0 ]; then
        fn_print_error "Failed to download: ${s_filename}"
        return 1
    fi  ## End if
    
    fn_print_info "Download completed: ${s_filename}"
    return 0
}  ## End Function fn_download_source

fn_extract_tarball() {  ## Local Function
    local s_filename="$1"
    local s_source_dir="$2"
    local s_extract_dir="$3"
    
    fn_print_info "Extracting: ${s_filename}"
    
    if [ -d "${s_extract_dir}" ]; then
        fn_print_info "Already extracted, skipping."
        return 0
    fi  ## End if
    
    tar -xzf "${s_source_dir}/${s_filename}" -C "${SRC_DIR}"
    
    if [ $? -ne 0 ]; then
        fn_print_error "Failed to extract: ${s_filename}"
        return 1
    fi  ## End if
    
    fn_print_info "Extraction completed."
    return 0
}  ## End Function fn_extract_tarball

fn_build_x265() {  ## Local Function
    fn_print_info "Building x265 (HEVC codec) for Debian 9..."
    
    local s_x265_src="${SRC_DIR}/x265_${X265_VERSION}"
    local s_x265_build="${BUILD_DIR}/x265"
    
    # Check if already built
    if [ -f "${s_x265_build}/libx265.a" ]; then
        fn_print_info "x265 already built, skipping."
        return 0
    fi  ## End if
    
    # Download if needed
    local s_x265_tar="x265_${X265_VERSION}.tar.gz"
    if ! fn_download_source "${X265_URL}" "${s_x265_tar}" "${SRC_DIR}"; then
        return 1
    fi  ## End if
    
    # Extract
    if ! fn_extract_tarball "${s_x265_tar}" "${SRC_DIR}" "${SRC_DIR}"; then
        return 1
    fi  ## End if
    
    # Configure and build
    cd "${s_x265_src}/source"
    
    fn_print_info "Configuring x265..."
    cmake -G "Unix Makefiles" \
        -DCMAKE_BUILD_TYPE=Release \
        -DENABLE_SHARED=OFF \
        -DENABLE_CLI=OFF \
        -DSTATIC_LINK_CRT=ON \
        -DENABLE_PIC=ON \
        -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
        -DCMAKE_CXX_FLAGS="-std=c++11" \
        -B "${s_x265_build}"
    
    if [ $? -ne 0 ]; then
        fn_print_error "Failed to configure x265"
        return 1
    fi  ## End if
    
    fn_print_info "Building x265..."
    cmake --build "${s_x265_build}" --config Release -j$(nproc)
    
    if [ $? -ne 0 ]; then
        fn_print_error "Failed to build x265"
        return 1
    fi  ## End if
    
    # Copy built files
    mkdir -p "${CODECS_DIR}/x265"
    cp "${s_x265_build}/libx265.a" "${CODECS_DIR}/x265/"
    cp -r "${s_x265_src}/source/x265.h" "${CODECS_DIR}/x265/"
    cp -r "${s_x265_src}/source/x265_config.h" "${CODECS_DIR}/x265/"
    
    fn_print_info "x265 built successfully."
    return 0
}  ## End Function fn_build_x265

fn_build_libheif() {  ## Local Function
    fn_print_info "Building libheif (HEIC/HEIF codec) for Debian 9..."
    
    local s_libheif_src="${SRC_DIR}/libheif-${LIBHEIF_VERSION}"
    local s_libheif_build="${BUILD_DIR}/libheif"
    
    # Check if already built
    if [ -f "${s_libheif_build}/lib/libheif.a" ]; then
        fn_print_info "libheif already built, skipping."
        return 0
    fi  ## End if
    
    # Download if needed
    local s_libheif_tar="libheif-${LIBHEIF_VERSION}.tar.gz"
    if ! fn_download_source "${LIBHEIF_URL}" "${s_libheif_tar}" "${SRC_DIR}"; then
        return 1
    fi  ## End if
    
    # Extract
    if ! fn_extract_tarball "${s_libheif_tar}" "${SRC_DIR}" "${SRC_DIR}"; then
        return 1
    fi  ## End if
    
    # Configure and build
    cd "${s_libheif_src}"
    
    fn_print_info "Configuring libheif..."
    
    # For Debian 9, we need to use older C++ standard
    export CXXFLAGS="-std=c++11"
    
    ./configure \
        --prefix="${s_libheif_build}" \
        --disable-shared \
        --enable-static \
        --disable-examples \
        --disable-go \
        --disable-multithreading \
        --with-libde265=no \
        --with-x265=yes \
        --with-pic \
        CFLAGS="-I${CODECS_DIR}/x265" \
        CXXFLAGS="-I${CODECS_DIR}/x265 -std=c++11" \
        LDFLAGS="-L${CODECS_DIR}/x265 -lx265"
    
    if [ $? -ne 0 ]; then
        fn_print_error "Failed to configure libheif"
        return 1
    fi  ## End if
    
    fn_print_info "Building libheif..."
    make -j$(nproc)
    
    if [ $? -ne 0 ]; then
        fn_print_error "Failed to build libheif"
        return 1
    fi  ## End if
    
    fn_print_info "Installing libheif..."
    make install
    
    # Copy built files
    mkdir -p "${CODECS_DIR}/libheif"
    cp "${s_libheif_build}/lib/libheif.a" "${CODECS_DIR}/libheif/"
    cp -r "${s_libheif_build}/include/libheif" "${CODECS_DIR}/libheif/include/"
    
    fn_print_info "libheif built successfully."
    return 0
}  ## End Function fn_build_libheif

fn_create_cmake_config() {  ## Local Function
    fn_print_info "Creating CMake configuration for embedded codecs..."
    
    cat > "${CODECS_DIR}/heif_embedded.cmake" << 'EOF'
# heif_embedded.cmake
# CMake configuration for embedded HEIC/HEIF codecs (Debian 9)
# Auto-generated by build_codecs_debian9.sh

# Embedded codec paths
set(EMBEDDED_X265_DIR "${CMAKE_CURRENT_LIST_DIR}/x265")
set(EMBEDDED_LIBHEIF_DIR "${CMAKE_CURRENT_LIST_DIR}/libheif")

# X265 library
add_library(x265_embedded STATIC IMPORTED)
set_target_properties(x265_embedded PROPERTIES
    IMPORTED_LOCATION "${EMBEDDED_X265_DIR}/libx265.a"
    INTERFACE_INCLUDE_DIRECTORIES "${EMBEDDED_X265_DIR}"
    INTERFACE_COMPILE_FEATURES cxx_std_11
)

# Libheif library
add_library(heif_embedded STATIC IMPORTED)
set_target_properties(heif_embedded PROPERTIES
    IMPORTED_LOCATION "${EMBEDDED_LIBHEIF_DIR}/libheif.a"
    INTERFACE_INCLUDE_DIRECTORIES "${EMBEDDED_LIBHEIF_DIR}/include"
    INTERFACE_COMPILE_FEATURES cxx_std_11
)

# Link libraries
target_link_libraries(heif_embedded INTERFACE x265_embedded)

# Add definitions for embedded codecs
add_definitions(-DHAVE_EMBEDDED_HEIC_CODECS)
add_definitions(-DHAVE_X265)
add_definitions(-DHAVE_LIBHEIF)

# Export variables for parent CMakeLists
set(EMBEDDED_CODECS_FOUND TRUE CACHE BOOL "Embedded HEIC codecs found")
set(EMBEDDED_X265_INCLUDE_DIRS "${EMBEDDED_X265_DIR}" CACHE PATH "X265 include directories")
set(EMBEDDED_LIBHEIF_INCLUDE_DIRS "${EMBEDDED_LIBHEIF_DIR}/include" CACHE PATH "Libheif include directories")
EOF
    
    fn_print_info "CMake configuration created: ${CODECS_DIR}/heif_embedded.cmake"
}  ## End Function fn_create_cmake_config

fn_create_pkgconfig() {  ## Local Function
    fn_print_info "Creating pkg-config files..."
    
    # Create x265.pc
    cat > "${CODECS_DIR}/x265.pc" << EOF
prefix=${CODECS_DIR}/x265
exec_prefix=\${prefix}
libdir=\${prefix}
includedir=\${prefix}

Name: x265
Description: H.265/HEVC video encoder
Version: ${X265_VERSION}
Libs: -L\${libdir} -lx265
Libs.private: -lstdc++ -lm -lrt -ldl
Cflags: -I\${includedir} -std=c++11
EOF
    
    # Create libheif.pc
    cat > "${CODECS_DIR}/libheif.pc" << EOF
prefix=${CODECS_DIR}/libheif
exec_prefix=\${prefix}
libdir=\${prefix}
includedir=\${prefix}/include

Name: libheif
Description: HEIF image codec
Version: ${LIBHEIF_VERSION}
Requires.private: x265
Libs: -L\${libdir} -lheif
Libs.private: -lx265 -lstdc++ -lm
Cflags: -I\${includedir} -std=c++11
EOF
    
    fn_print_info "pkg-config files created."
}  ## End Function fn_create_pkgconfig

fn_main() {  ## Local Function
    fn_print_info "========================================"
    fn_print_info "Building Embedded HEIC/HEIF Codecs"
    fn_print_info "System: Debian 9"
    fn_print_info "Version: v1.0"
    fn_print_info "========================================"
    echo ""
    
    # Check dependencies
    fn_check_dependencies
    
    # Create directories
    fn_create_directories
    
    # Build x265
    if ! fn_build_x265; then
        fn_print_error "Failed to build x265. Exiting."
        exit 1
    fi  ## End if
    
    # Build libheif
    if ! fn_build_libheif; then
        fn_print_error "Failed to build libheif. Exiting."
        exit 1
    fi  ## End if
    
    # Create CMake config
    fn_create_cmake_config
    
    # Create pkg-config files
    fn_create_pkgconfig
    
    fn_print_info "========================================"
    fn_print_info "Codec Build Summary"
    fn_print_info "========================================"
    fn_print_info "Built components:"
    fn_print_info "  - x265 (HEVC codec): Version ${X265_VERSION}"
    fn_print_info "  - libheif (HEIC codec): Version ${LIBHEIF_VERSION}"
    fn_print_info ""
    fn_print_info "Note: Built with C++11 for Debian 9 compatibility"
    fn_print_info ""
    fn_print_info "Output directory: ${CODECS_DIR}"
    fn_print_info "========================================"
    
    fn_print_info "Embedded codec build completed successfully!"
}  ## End Function fn_main

# Run main function
fn_main

exit 0