#!/bin/bash
#
# install_deps_debian9.sh
# Dependency installer for HEIC Converter on Debian 9 (Stretch)
# Author: R Square Innovation Software
# Version: v1.0

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Function to print colored output
fn_print_status() {
    echo -e "${GREEN}[STATUS]${NC} $1"
}

fn_print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

fn_print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Function to check if running as root
fn_check_root() {
    if [ "$EUID" -ne 0 ]; then
        fn_print_error "This script must be run as root. Please use sudo."
        exit 1
    fi
}

# Function to install build dependencies
fn_install_build_deps() {
    fn_print_status "Installing build dependencies..."
    
    apt-get install -y \
        build-essential \
        cmake \
        pkg-config \
        git \
        wget \
        curl \
        tar \
        gzip \
        bzip2 \
        unzip \
        ninja-build
    
    # Check if installation was successful
    if [ $? -eq 0 ]; then
        fn_print_status "Build dependencies installed successfully."
    else
        fn_print_error "Failed to install build dependencies."
        exit 1
    fi
}

# Function to install image format libraries (system codecs)
fn_install_image_libs() {
    fn_print_status "Installing image format libraries..."
    
    # Note: We are NOT installing libheif or libx265 as they will be embedded
    # We only install libraries for output formats
    
    apt-get install -y \
        libjpeg-dev \
        libpng-dev \
        libtiff-dev \
        libwebp-dev \
        libopenjp2-7-dev \
        libraw-dev
    
    # For Debian 9, libavif might not be available, so we'll build it from source if needed
    # but for now, we'll skip it as it's not commonly used
    
    # Check if installation was successful
    if [ $? -eq 0 ]; then
        fn_print_status "Image format libraries installed successfully."
    else
        fn_print_warning "Some image format libraries failed to install."
        fn_print_warning "The converter will work with available formats only."
    fi
}

# Function to install additional development libraries
fn_install_dev_libs() {
    fn_print_status "Installing additional development libraries..."
    
    apt-get install -y \
        libstdc++6 \
        libgcc1 \
        zlib1g-dev \
        libxml2-dev \
        libexif-dev \
        liblcms2-dev \
        libde265-dev  # Only needed for headers, not for runtime
    
    # Note: We install libde265-dev for headers but we'll use embedded version
    
    # Check if installation was successful
    if [ $? -eq 0 ]; then
        fn_print_status "Development libraries installed successfully."
    else
        fn_print_warning "Some development libraries failed to install."
    fi
}

# Function to install testing dependencies (optional)
fn_install_test_deps() {
    fn_print_status "Installing testing dependencies (optional)..."
    
    apt-get install -y \
        valgrind \
        cppcheck \
        gdb \
        imagemagick  # For creating test images
    
    # Check if installation was successful
    if [ $? -eq 0 ]; then
        fn_print_status "Testing dependencies installed successfully."
    else
        fn_print_warning "Some testing dependencies failed to install."
        fn_print_warning "Testing features may not work properly."
    fi
}

# Function to create necessary directories
fn_create_dirs() {
    fn_print_status "Creating project directories..."
    
    # Create directories for embedded codecs
    mkdir -p /opt/heic_converter/embedded
    mkdir -p /var/log/heic_converter
    
    # Set proper permissions
    chmod 755 /opt/heic_converter
    chmod 755 /var/log/heic_converter
    
    fn_print_status "Directories created successfully."
}

# Function to configure system for building
fn_configure_system() {
    fn_print_status "Configuring system for building..."
    
    # Increase system limits for building large libraries
    echo "fs.file-max = 2097152" >> /etc/sysctl.conf
    sysctl -p
    
    # Update ldconfig cache
    ldconfig
    
    fn_print_status "System configuration completed."
}

# Function to verify installations
fn_verify_installations() {
    fn_print_status "Verifying installations..."
    
    local bAllOK=true
    
    # Check for essential tools
    for tool in gcc g++ cmake make pkg-config; do
        if ! command -v $tool &> /dev/null; then
            fn_print_error "$tool not found!"
            bAllOK=false
        fi
    done
    
    # Check for libraries
    for lib in libjpeg libpng libtiff; do
        if ! pkg-config --exists $lib; then
            fn_print_warning "$lib development files not found!"
            bAllOK=false
        fi
    done
    
    if [ "$bAllOK" = true ]; then
        fn_print_status "All essential dependencies are installed."
        return 0
    else
        fn_print_warning "Some dependencies are missing. The build may fail."
        return 1
    fi
}

# Function to display summary
fn_display_summary() {
    echo ""
    echo "========================================="
    echo "DEPENDENCY INSTALLATION SUMMARY"
    echo "========================================="
    echo "System: Debian 9 (Stretch)"
    echo "Project: HEIC Converter v1.0"
    echo "Author: R Square Innovation Software"
    echo ""
    echo "The following components were installed:"
    echo "1. Build tools (gcc, g++, cmake, make)"
    echo "2. Image format libraries (JPEG, PNG, TIFF, WebP)"
    echo "3. Development libraries"
    echo "4. Optional testing tools"
    echo ""
    echo "Note: HEIC/HEIF codecs (libheif, x265) are NOT"
    echo "      installed system-wide. They will be"
    echo "      embedded in the application."
    echo ""
    echo "Next steps:"
    echo "1. Run the build script: ./scripts/build_debian9.sh"
    echo "2. Or build manually with CMake"
    echo "========================================="
}

# Main function
fn_main() {
    fn_print_status "Starting dependency installation for Debian 9..."
    fn_print_status "This script will install system dependencies for HEIC Converter."
    echo ""
    
    # Check if running as root
    fn_check_root
    
    # Install dependencies
    fn_install_build_deps
    fn_install_image_libs
    fn_install_dev_libs
    
    # Ask about test dependencies
    echo ""
    read -p "Install testing dependencies? (y/n): " -n 1 -r
    echo ""
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        fn_install_test_deps
    fi
    
    # Create directories
    fn_create_dirs
    
    # Configure system
    fn_configure_system
    
    # Verify installations
    fn_verify_installations
    
    # Display summary
    fn_display_summary
    
    fn_print_status "Dependency installation completed!"
    fn_print_status "You can now build the HEIC Converter."
}

# Handle script arguments
case "$1" in
    --help|-h)
        echo "Usage: $0 [OPTION]"
        echo "Install dependencies for HEIC Converter on Debian 9"
        echo ""
        echo "Options:"
        echo "  -h, --help    Show this help message"
        echo "  -v, --version Show version information"
        echo ""
        echo "Note: This script must be run as root (use sudo)"
        exit 0
        ;;
    --version|-v)
        echo "HEIC Converter Dependency Installer v1.0"
        echo "For Debian 9 (Stretch)"
        echo "Author: R Square Innovation Software"
        exit 0
        ;;
    *)
        fn_main
        ;;
esac

exit 0