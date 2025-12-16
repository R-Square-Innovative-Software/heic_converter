#!/bin/bash

# heic_converter_v1.0 - Build Script for Debian 9 (Stretch)
# Author: R Square Innovation Software
# Version: 1.0
# Description: Build script for HEIC/HEIF converter on Debian 9 systems
# This script embeds HEIC codecs and uses system libraries for other formats

# Exit on error
set -e

# Color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
PROJECT_NAME="heic_converter"
VERSION="v1.0"
BUILD_DIR="build_debian9"
INSTALL_DIR="/usr/local"
DATA_DIR="data/debian9"
EMBEDDED_LIBS_DIR="$BUILD_DIR/embedded_libs"

# Function to print colored output
fn_print_message() { # Local Function
  local sMessage=$1
  local sColor=$2
  echo -e "${sColor}${sMessage}${NC}"
} # End Function fn_print_message

# Function to check if running as root
fn_check_root() { # Local Function
  if [ "$EUID" -ne 0 ]; then
    fn_print_message "This script requires root privileges for installing dependencies." "$YELLOW"
    fn_print_message "Please run with sudo or as root." "$YELLOW"
    exit 1
  fi
} # End Function fn_check_root

# Function to install build dependencies
fn_install_dependencies() { # Local Function
  fn_print_message "Installing build dependencies for Debian 9..." "$BLUE"
  
  # Install compiler and build tools
  apt-get install -y \
    gcc \
    g++ \
    cmake \
    make \
    pkg-config \
    git \
    wget \
    curl
  
  # Install system libraries for output formats
  apt-get install -y \
    libjpeg-dev \
    libpng-dev \
    libtiff-dev \
    libwebp-dev
  
  # Install additional required libraries
  apt-get install -y \
    libstdc++6 \
    zlib1g-dev
  
  fn_print_message "Build dependencies installed successfully." "$GREEN"
} # End Function fn_install_dependencies

# Function to setup embedded codecs
fn_setup_embedded_codecs() { # Local Function
  fn_print_message "Setting up embedded HEIC codecs for Debian 9..." "$BLUE"
  
  # Create build directory
  mkdir -p "$BUILD_DIR"
  mkdir -p "$EMBEDDED_LIBS_DIR"
  
  # Check if embedded codec sources exist
  if [ ! -d "$DATA_DIR/libheif" ] || [ ! -d "$DATA_DIR/x265" ]; then
    fn_print_message "Error: Embedded codec sources not found in $DATA_DIR" "$RED"
    fn_print_message "Please ensure the data directory contains libheif and x265 sources." "$RED"
    exit 1
  fi
  
  # Copy embedded codec sources to build directory
  cp -r "$DATA_DIR/libheif" "$EMBEDDED_LIBS_DIR/"
  cp -r "$DATA_DIR/x265" "$EMBEDDED_LIBS_DIR/"
  
  # Set up environment variables for embedded codecs
  export EMBEDDED_CODECS_PATH="$PWD/$EMBEDDED_LIBS_DIR"
  export PKG_CONFIG_PATH="$EMBEDDED_CODECS_PATH/lib/pkgconfig:$PKG_CONFIG_PATH"
  export LD_LIBRARY_PATH="$EMBEDDED_CODECS_PATH/lib:$LD_LIBRARY_PATH"
  
  fn_print_message "Embedded codecs setup completed." "$GREEN"
} # End Function fn_setup_embedded_codecs

# Function to build x265
fn_build_x265() { # Local Function
  fn_print_message "Building embedded x265 codec..." "$BLUE"
  
  local sX265Dir="$EMBEDDED_LIBS_DIR/x265"
  
  # Create and enter build directory
  mkdir -p "$sX265Dir/build"
  cd "$sX265Dir/build"
  
  # Configure x265 with minimal options for compatibility
  cmake \
    -DCMAKE_INSTALL_PREFIX="$EMBEDDED_CODECS_PATH" \
    -DENABLE_SHARED=ON \
    -DENABLE_CLI=OFF \
    -DENABLE_HDR10_PLUS=OFF \
    -DEXTRA_LIB="x265_main10.a;x265_main12.a" \
    -DEXTRA_LINK_FLAGS="-L." \
    -DLINKED_10BIT=ON \
    -DLINKED_12BIT=ON \
    -DCMAKE_BUILD_TYPE=Release \
    ..
  
  # Build and install
  make -j$(nproc)
  make install
  
  # Return to project root
  cd ../../../
  
  fn_print_message "x265 built successfully." "$GREEN"
} # End Function fn_build_x265

# Function to build libheif
fn_build_libheif() { # Local Function
  fn_print_message "Building embedded libheif..." "$BLUE"
  
  local sLibheifDir="$EMBEDDED_LIBS_DIR/libheif"
  
  # Create and enter build directory
  mkdir -p "$sLibheifDir/build"
  cd "$sLibheifDir/build"
  
  # Configure libheif with embedded x265
  cmake \
    -DCMAKE_INSTALL_PREFIX="$EMBEDDED_CODECS_PATH" \
    -DWITH_EXAMPLES=OFF \
    -DWITH_RAV1E=OFF \
    -DWITH_SvtEnc=OFF \
    -DWITH_DAV1D=OFF \
    -DWITH_AOM_DECODER=OFF \
    -DWITH_AOM_ENCODER=OFF \
    -DWITH_FUZZERS=OFF \
    -DWITH_LIBDE265=OFF \
    -DWITH_X265=ON \
    -DX265_LIBRARY="$EMBEDDED_CODECS_PATH/lib/libx265.so" \
    -DX265_INCLUDE_DIR="$EMBEDDED_CODECS_PATH/include" \
    -DCMAKE_BUILD_TYPE=Release \
    ..
  
  # Build and install
  make -j$(nproc)
  make install
  
  # Return to project root
  cd ../../../
  
  fn_print_message "libheif built successfully." "$GREEN"
} # End Function fn_build_libheif

# Function to build main project
fn_build_project() { # Local Function
  fn_print_message "Building HEIC Converter v1.0 for Debian 9..." "$BLUE"
  
  # Create and enter build directory
  mkdir -p "$BUILD_DIR/project"
  cd "$BUILD_DIR/project"
  
  # Configure project with embedded codecs
  cmake \
    ../.. \
    -DCMAKE_INSTALL_PREFIX="$INSTALL_DIR" \
    -DDEBIAN_VERSION=9 \
    -DEMBEDDED_HEIC_CODECS=ON \
    -DEMBEDDED_CODECS_PATH="$EMBEDDED_CODECS_PATH" \
    -DUSE_SYSTEM_JPEG=ON \
    -DUSE_SYSTEM_PNG=ON \
    -DUSE_SYSTEM_TIFF=ON \
    -DUSE_SYSTEM_WEBP=ON \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_STANDARD=11 \
    -DCMAKE_CXX_FLAGS="-Wno-deprecated-declarations"
  
  # Build project
  make -j$(nproc)
  
  # Return to project root
  cd ../../
  
  fn_print_message "Project built successfully." "$GREEN"
} # End Function fn_build_project

# Function to run tests
fn_run_tests() { # Local Function
  fn_print_message "Running tests..." "$BLUE"
  
  cd "$BUILD_DIR/project"
  
  # Check if tests were built
  if [ -f "heic_converter_tests" ]; then
    ./heic_converter_tests
    fn_print_message "Tests completed." "$GREEN"
  else
    fn_print_message "Test binary not found, skipping tests." "$YELLOW"
  fi
  
  cd ../../
} # End Function fn_run_tests

# Function to install the program
fn_install_program() { # Local Function
  fn_print_message "Installing HEIC Converter to $INSTALL_DIR..." "$BLUE"
  
  cd "$BUILD_DIR/project"
  
  # Install the program
  make install
  
  # Create symbolic links for easier access
  ln -sf "$INSTALL_DIR/bin/heic_converter" "/usr/local/bin/heic_converter"
  
  # Copy embedded libraries to system library path
  mkdir -p "/usr/local/lib/heic_converter"
  cp "$EMBEDDED_CODECS_PATH/lib/"libheif* "/usr/local/lib/heic_converter/"
  cp "$EMBEDDED_CODECS_PATH/lib/"libx265* "/usr/local/lib/heic_converter/"
  
  # Update ldconfig for embedded libraries
  echo "/usr/local/lib/heic_converter" > /etc/ld.so.conf.d/heic_converter.conf
  ldconfig
  
  cd ../../
  
  fn_print_message "Installation completed successfully." "$GREEN"
  fn_print_message "HEIC Converter v1.0 is now installed." "$GREEN"
  fn_print_message "Usage: heic_converter [options] <input_files>" "$GREEN"
} # End Function fn_install_program

# Function to create standalone bundle
fn_create_bundle() { # Local Function
  fn_print_message "Creating standalone bundle..." "$BLUE"
  
  local sBundleDir="$BUILD_DIR/bundle"
  mkdir -p "$sBundleDir"
  
  # Copy executable
  cp "$BUILD_DIR/project/heic_converter" "$sBundleDir/"
  
  # Copy embedded libraries
  mkdir -p "$sBundleDir/lib"
  cp "$EMBEDDED_CODECS_PATH/lib/"libheif*.so* "$sBundleDir/lib/"
  cp "$EMBEDDED_CODECS_PATH/lib/"libx265*.so* "$sBundleDir/lib/"
  
  # Create wrapper script
  cat > "$sBundleDir/heic_converter_wrapper.sh" << 'EOF'
#!/bin/bash
# Wrapper script for standalone HEIC Converter
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
export LD_LIBRARY_PATH="$SCRIPT_DIR/lib:$LD_LIBRARY_PATH"
"$SCRIPT_DIR/heic_converter" "$@"
EOF
  
  chmod +x "$sBundleDir/heic_converter_wrapper.sh"
  
  # Create README
  cat > "$sBundleDir/README.txt" << EOF
HEIC Converter v1.0 - Standalone Bundle for Debian 9
===================================================

This is a standalone version of the HEIC Converter that includes
all necessary HEIC/HEIF codecs embedded.

Usage:
  ./heic_converter_wrapper.sh [options] <input_files>

For help:
  ./heic_converter_wrapper.sh --help

This bundle includes:
  - heic_converter: Main executable
  - Embedded HEIC/HEIF decoding libraries
  - Wrapper script that sets up library paths

Author: R Square Innovation Software
EOF
  
  # Create tarball
  tar -czf "heic_converter_debian9_$VERSION.tar.gz" -C "$BUILD_DIR" bundle
  
  fn_print_message "Standalone bundle created: heic_converter_debian9_$VERSION.tar.gz" "$GREEN"
} # End Function fn_create_bundle

# Function to clean build files
fn_clean_build() { # Local Function
  fn_print_message "Cleaning build files..." "$BLUE"
  
  if [ -d "$BUILD_DIR" ]; then
    rm -rf "$BUILD_DIR"
    fn_print_message "Build directory cleaned." "$GREEN"
  else
    fn_print_message "Build directory does not exist." "$YELLOW"
  fi
} # End Function fn_clean_build

# Function to display help
fn_show_help() { # Local Function
  echo "HEIC Converter v1.0 - Build Script for Debian 9"
  echo "Usage: $0 [OPTION]"
  echo ""
  echo "Options:"
  echo "  all              Install dependencies and build everything (default)"
  echo "  deps             Install build dependencies only"
  echo "  build            Build project only (assumes dependencies are installed)"
  echo "  test             Build and run tests"
  echo "  install          Build and install to system"
  echo "  bundle           Create standalone bundle"
  echo "  clean            Clean build files"
  echo "  help             Show this help message"
  echo ""
  echo "Examples:"
  echo "  $0 all           # Complete build and install"
  echo "  $0 build         # Build project only"
  echo "  $0 clean         # Clean build directory"
} # End Function fn_show_help

# Main function
fn_main() { # Local Function
  local sAction=${1:-"all"}
  
  case "$sAction" in
    "all")
      fn_check_root
      fn_install_dependencies
      fn_setup_embedded_codecs
      fn_build_x265
      fn_build_libheif
      fn_build_project
      fn_run_tests
      fn_install_program
      ;;
      
    "deps")
      fn_check_root
      fn_install_dependencies
      ;;
      
    "build")
      fn_setup_embedded_codecs
      fn_build_x265
      fn_build_libheif
      fn_build_project
      ;;
      
    "test")
      fn_setup_embedded_codecs
      fn_build_x265
      fn_build_libheif
      fn_build_project
      fn_run_tests
      ;;
      
    "install")
      fn_check_root
      fn_setup_embedded_codecs
      fn_build_x265
      fn_build_libheif
      fn_build_project
      fn_install_program
      ;;
      
    "bundle")
      fn_setup_embedded_codecs
      fn_build_x265
      fn_build_libheif
      fn_build_project
      fn_create_bundle
      ;;
      
    "clean")
      fn_clean_build
      ;;
      
    "help"|"-h"|"--help")
      fn_show_help
      ;;
      
    *)
      fn_print_message "Unknown option: $sAction" "$RED"
      fn_show_help
      exit 1
      ;;
  esac
} # End Function fn_main

# Run main function with arguments
fn_main "$@"

fn_print_message "Build script completed." "$GREEN"