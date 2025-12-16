#!/bin/bash
#
# install_deps_debian12.sh
# HEIC/HEIF Converter - Dependency Installer for Debian 12/13
# Version: v1.0
# Author: R Square Innovation Software
#
# This script installs system dependencies for the HEIC/HEIF converter
# on Debian 12 (Bookworm) and Debian 13 (Trixie) systems.
# Note: HEIC/HEIF codecs are embedded, only output format libraries are installed.

set -e  # Exit on error

# Color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Function definitions
fn_print_info() {  # Local Function
    echo -e "${GREEN}[INFO]${NC} $1"
}  # End Function fn_print_info

fn_print_warning() {  # Local Function
    echo -e "${YELLOW}[WARNING]${NC} $1"
}  # End Function fn_print_warning

fn_print_error() {  # Local Function
    echo -e "${RED}[ERROR]${NC} $1"
}  # End Function fn_print_error

fn_check_root() {  # Local Function
    if [[ $EUID -ne 0 ]]; then
        fn_print_error "This script must be run as root. Use sudo."
        exit 1
    fi  # End if
}  ## End Function fn_check_root

fn_install_build_tools() {  ## Local Function
    fn_print_info "Installing build tools and development libraries..."
    
    # Essential build tools
    apt-get install -y \
        build-essential \
        cmake \
        pkg-config \
        git \
        ninja-build
    
    # C++ development
    apt-get install -y \
        g++ \
        gcc \
        make
}  ## End Function fn_install_build_tools

fn_install_image_libraries() {  ## Local Function
    fn_print_info "Installing image format libraries (output formats only)..."
    
    # JPEG support (using system libjpeg-turbo)
    apt-get install -y \
        libjpeg-dev \
        libjpeg-turbo8-dev
    
    # PNG support
    apt-get install -y \
        libpng-dev
    
    # TIFF support - CORRECTED for Debian 12/13
    apt-get install -y \
        libtiff-dev
    
    # WebP support
    apt-get install -y \
        libwebp-dev
    
    # For testing and additional formats
    apt-get install -y \
        libgd-dev \
        imagemagick  # For testing conversions
    
    fn_print_info "Image format libraries installed successfully."
}  ## End Function fn_install_image_libraries

fn_install_system_deps() {  ## Local Function
    fn_print_info "Installing additional system dependencies..."
    
    # General utilities
    apt-get install -y \
        wget \
        curl \
        unzip \
        file \
        libssl-dev \
        zlib1g-dev
    
    # For embedded codec building
    apt-get install -y \
        nasm \
        yasm
    
    fn_print_info "System dependencies installed successfully."
}  ## End Function fn_install_system_deps

fn_cleanup() {  ## Local Function
    fn_print_info "Cleaning up package cache..."
    apt-get autoremove -y
    apt-get clean
    
    fn_print_info "Cleaning up temporary files..."
    rm -rf /var/lib/apt/lists/*
}  ## End Function fn_cleanup

fn_verify_installations() {  ## Local Function
    fn_print_info "Verifying installations..."
    
    # Check for required libraries
    local vs_required_libs=(
        "libjpeg"
        "libpng"
        "libtiff"
        "libwebp"
    )
    
    local b_all_installed=true
    
    for s_lib in "${vs_required_libs[@]}"; do
        if ! pkg-config --exists "${s_lib}"; then
            fn_print_warning "${s_lib} development files not found via pkg-config"
            b_all_installed=false
        else
            fn_print_info "${s_lib} development files are available"
        fi  ## End if
    done  ## End for
    
    # Check if libraries are available in system
    local vs_lib_files=(
        "/usr/include/jpeglib.h"
        "/usr/include/png.h"
        "/usr/include/tiff.h"
        "/usr/include/webp/decode.h"
    )
    
    local s_lib_file
    for s_lib_file in "${vs_lib_files[@]}"; do
        if [ -f "$s_lib_file" ]; then
            fn_print_info "Found: $s_lib_file"
        else
            fn_print_warning "Missing: $s_lib_file"
            b_all_installed=false
        fi  ## End if
    done  ## End for
    
    if [ "$b_all_installed" = true ]; then
        fn_print_info "All required libraries are installed."
    else
        fn_print_warning "Some libraries may need manual installation."
    fi  ## End if
}  ## End Function fn_verify_installations

fn_print_summary() {  ## Local Function
    echo ""
    echo "================================================"
    echo "Dependency Installation Summary"
    echo "================================================"
    echo "System: Debian 12/13"
    echo "Build tools: Installed"
    echo "Output format libraries: Installed"
    echo "HEIC/HEIF codecs: Will be embedded (not installed)"
    echo ""
    echo "Next steps:"
    echo "1. Run the build script: ./scripts/build_debian12.sh"
    echo "2. Or run manually:"
    echo "   mkdir build && cd build"
    echo "   cmake .. -DDEBIAN12_BUILD=ON"
    echo "   make"
    echo "================================================"
    echo ""
}  ## End Function fn_print_summary

fn_main() {  ## Local Function
    fn_print_info "Starting dependency installation for Debian 12/13"
    fn_print_info "HEIC/HEIF codecs will be embedded, not installed system-wide"
    
    # Check if running as root
    fn_check_root
    
    # Install build tools
    fn_install_build_tools
    
    # Install image libraries (output formats only)
    fn_install_image_libraries
    
    # Install additional dependencies
    fn_install_system_deps
    
    # Cleanup
    fn_cleanup
    
    # Verify installations
    fn_verify_installations
    
    # Print summary
    fn_print_summary
    
    fn_print_info "Dependency installation completed successfully!"
}  ## End Function fn_main

# Check if running in interactive mode
if [ -t 0 ]; then
    echo "================================================"
    echo "HEIC/HEIF Converter - Debian 12/13 Dependencies"
    echo "Version: v1.0"
    echo "Author: R Square Innovation Software"
    echo "================================================"
    echo ""
    echo "This script will install system dependencies for building"
    echo "the HEIC/HEIF converter. HEIC/HEIF codecs are embedded,"
    echo "only output format libraries will be installed."
    echo ""
    echo "The following will be installed:"
    echo "- Build tools (g++, CMake, make, etc.)"
    echo "- Image format libraries (libjpeg, libpng, libtiff, etc.)"
    echo "- System dependencies"
    echo ""
    
    read -p "Continue? (y/n): " s_choice
    case "$s_choice" in
        y|Y)
            fn_main
            ;;
        n|N)
            fn_print_info "Installation cancelled."
            exit 0
            ;;
        *)
            fn_print_error "Invalid choice. Exiting."
            exit 1
            ;;
    esac  ## End case
else
    # Non-interactive mode
    fn_main
fi  ## End if

exit 0