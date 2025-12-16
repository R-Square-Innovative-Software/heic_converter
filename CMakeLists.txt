cmake_minimum_required(VERSION 3.10)
project(heic_converter VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build configuration
if(DEFINED DEBIAN9_BUILD)
    add_definitions(-DDEBIAN9_BUILD)
    set(BUILD_TYPE "debian9")
    message(STATUS "Building for Debian 9")
elseif(DEFINED DEBIAN12_BUILD)
    add_definitions(-DDEBIAN12_BUILD)
    set(BUILD_TYPE "debian12")
    message(STATUS "Building for Debian 12/13")
else()
    message(WARNING "No build type specified. Using generic build.")
    set(BUILD_TYPE "generic")
endif()

message(STATUS "Build type: ${BUILD_TYPE}")

# Find required system libraries
find_package(PNG REQUIRED)
find_package(JPEG REQUIRED)

# WebP library - handle different CMake versions and Debian packaging
find_package(WebP)
if(WebP_FOUND)
    message(STATUS "WebP support: YES")
    add_definitions(-DHAVE_WEBP)
    set(WEBP_LIBRARIES_FOUND TRUE)
else()
    # Try alternative method for Debian
    find_path(WEBP_INCLUDE_DIR webp/decode.h)
    find_library(WEBP_LIBRARY NAMES webp)
    find_library(WEBP_DEMUX_LIBRARY NAMES webpdemux)
    find_library(WEBP_MUX_LIBRARY NAMES webpmux)
    
    if(WEBP_INCLUDE_DIR AND WEBP_LIBRARY)
        message(STATUS "WebP support: YES (via manual detection)")
        add_definitions(-DHAVE_WEBP)
        set(WEBP_LIBRARIES_FOUND TRUE)
        set(WEBP_INCLUDE_DIRS ${WEBP_INCLUDE_DIR})
        set(WEBP_LIBRARIES ${WEBP_LIBRARY} ${WEBP_DEMUX_LIBRARY} ${WEBP_MUX_LIBRARY})
    else()
        message(WARNING "WebP library not found. WebP output will not be available.")
        set(WEBP_LIBRARIES_FOUND FALSE)
    endif()
endif()

# TIFF library
find_package(TIFF)
if(TIFF_FOUND)
    message(STATUS "TIFF support: YES")
    add_definitions(-DHAVE_TIFF)
    set(TIFF_LIBRARIES_FOUND TRUE)
else()
    # Try alternative method for Debian
    find_path(TIFF_INCLUDE_DIR tiff.h)
    find_library(TIFF_LIBRARY NAMES tiff)
    
    if(TIFF_INCLUDE_DIR AND TIFF_LIBRARY)
        message(STATUS "TIFF support: YES (via manual detection)")
        add_definitions(-DHAVE_TIFF)
        set(TIFF_LIBRARIES_FOUND TRUE)
        set(TIFF_INCLUDE_DIRS ${TIFF_INCLUDE_DIR})
        set(TIFF_LIBRARIES ${TIFF_LIBRARY})
    else()
        message(WARNING "TIFF library not found. TIFF output will not be available.")
        set(TIFF_LIBRARIES_FOUND FALSE)
    endif()
endif()

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Add source files
set(SOURCES
    src/main.cpp
    src/converter.cpp
    src/image_processor.cpp
    src/heic_decoder.cpp
    src/format_encoder.cpp
    src/batch_processor.cpp
    src/file_utils.cpp
    src/logger.cpp
    src/config.cpp
)

# Add executable
add_executable(heic_converter ${SOURCES})

# Link core libraries
target_link_libraries(heic_converter PRIVATE PNG::PNG JPEG::JPEG)

# Link WebP if found
if(WEBP_LIBRARIES_FOUND)
    if(TARGET WebP::WebP)
        target_link_libraries(heic_converter PRIVATE WebP::WebP)
    else()
        target_include_directories(heic_converter PRIVATE ${WEBP_INCLUDE_DIRS})
        target_link_libraries(heic_converter PRIVATE ${WEBP_LIBRARIES})
        message(STATUS "Linked WebP via manual configuration")
    endif()
endif()

# Link TIFF if found
if(TIFF_LIBRARIES_FOUND)
    if(TARGET TIFF::TIFF)
        target_link_libraries(heic_converter PRIVATE TIFF::TIFF)
    else()
        target_include_directories(heic_converter PRIVATE ${TIFF_INCLUDE_DIRS})
        target_link_libraries(heic_converter PRIVATE ${TIFF_LIBRARIES})
        message(STATUS "Linked TIFF via manual configuration")
    endif()
endif()

# Add math library (needed for some operations)
target_link_libraries(heic_converter PRIVATE m)

# Set output directory
set_target_properties(heic_converter PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    OUTPUT_NAME heic_converter
)

# Install target
install(TARGETS heic_converter
    RUNTIME DESTINATION bin
)

message(STATUS "Build configuration complete.")
message(STATUS "Target: heic_converter")
message(STATUS "Build type: ${BUILD_TYPE}")