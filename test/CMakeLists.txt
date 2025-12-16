# tests/CMakeLists.txt
# Test configuration for HEIC/HEIF Converter
# Author: R Square Innovation Software
# Version: v1.0

cmake_minimum_required(VERSION 3.10)
project(heic_converter_tests)

# Set compiler options
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable testing
enable_testing()

# Set test include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Set test source files
set(TEST_SOURCES
    test_heic_decoder.cpp
    test_format_encoder.cpp
    test_batch_processor.cpp
    test_file_utils.cpp
)

# Set test executable name
set(TEST_EXECUTABLE heic_converter_tests)

# Create test executable
add_executable(${TEST_EXECUTABLE} ${TEST_SOURCES})

# Link test executable with main project library
target_link_libraries(${TEST_EXECUTABLE} heic_converter)

# Find system libraries for testing
find_package(PNG REQUIRED)
target_link_libraries(${TEST_EXECUTABLE} ${PNG_LIBRARIES})

find_package(JPEG REQUIRED)
target_link_libraries(${TEST_EXECUTABLE} ${JPEG_LIBRARIES})

# Check for WebP support
find_package(WebP)
if(WebP_FOUND)
    target_link_libraries(${TEST_EXECUTABLE} ${WEBP_LIBRARIES})
    add_definitions(-DHAVE_WEBP)
endif()

# Check for TIFF support
find_package(TIFF)
if(TIFF_FOUND)
    target_link_libraries(${TEST_EXECUTABLE} ${TIFF_LIBRARIES})
    add_definitions(-DHAVE_TIFF)
endif()

# Link with pthread for multithreading
find_package(Threads REQUIRED)
target_link_libraries(${TEST_EXECUTABLE} Threads::Threads)

# Set test properties
set_target_properties(${TEST_EXECUTABLE} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    OUTPUT_NAME ${TEST_EXECUTABLE}
)

# Copy test data to binary directory
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/test_data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Define test cases
add_test(NAME test_heic_decoder COMMAND ${TEST_EXECUTABLE} --test-heic-decoder)
add_test(NAME test_format_encoder COMMAND ${TEST_EXECUTABLE} --test-format-encoder)
add_test(NAME test_batch_processor COMMAND ${TEST_EXECUTABLE} --test-batch-processor)
add_test(NAME test_file_utils COMMAND ${TEST_EXECUTABLE} --test-file-utils)
add_test(NAME test_all COMMAND ${TEST_EXECUTABLE} --test-all)

# Set test timeouts
set_tests_properties(test_heic_decoder PROPERTIES TIMEOUT 30)
set_tests_properties(test_format_encoder PROPERTIES TIMEOUT 30)
set_tests_properties(test_batch_processor PROPERTIES TIMEOUT 60)
set_tests_properties(test_file_utils PROPERTIES TIMEOUT 30)
set_tests_properties(test_all PROPERTIES TIMEOUT 120)

# Create custom target for running tests
add_custom_target(run_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS ${TEST_EXECUTABLE}
    COMMENT "Running tests..."
)

# Add test coverage support if available
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    # Check for coverage tools
    find_program(GCOV_PATH gcov)
    find_program(LCOV_PATH lcov)
    find_program(GENHTML_PATH genhtml)
    
    if(GCOV_PATH AND LCOV_PATH AND GENHTML_PATH)
        # Add coverage target
        add_custom_target(coverage
            # Clean previous coverage data
            COMMAND ${LCOV_PATH} --directory . --zerocounters
            # Run tests
            COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${TEST_EXECUTABLE}
            # Capture coverage data
            COMMAND ${LCOV_PATH} --directory . --capture --output-file coverage.info
            # Filter out system files
            COMMAND ${LCOV_PATH} --remove coverage.info '/usr/*' '*/tests/*' --output-file coverage.info
            # Generate HTML report
            COMMAND ${GENHTML_PATH} coverage.info --output-directory coverage_report
            # Clean up
            COMMAND rm -f coverage.info
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating test coverage report..."
        )
    endif()
endif()

# Add memory checking with Valgrind if available
find_program(VALGRIND_PATH valgrind)
if(VALGRIND_PATH)
    add_custom_target(memory_check
        COMMAND ${VALGRIND_PATH} --leak-check=full --show-leak-kinds=all --track-origins=yes --error-exitcode=1 ${CMAKE_CURRENT_BINARY_DIR}/${TEST_EXECUTABLE}
        DEPENDS ${TEST_EXECUTABLE}
        COMMENT "Running memory check with Valgrind..."
    )
endif()

# Add test configuration summary
message(STATUS "Test executable: ${TEST_EXECUTABLE}")
message(STATUS "Test source files: ${TEST_SOURCES}")
message(STATUS "Test data directory: ${CMAKE_CURRENT_SOURCE_DIR}/test_data")

# Platform-specific test configurations
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    # Linux-specific test configurations
    message(STATUS "Platform: Linux")
    
    # Check for older Debian 9
    if(EXISTS "/etc/debian_version")
        file(READ "/etc/debian_version" DEBIAN_VERSION)
        if(${DEBIAN_VERSION} MATCHES "9.*")
            message(STATUS "Debian 9 detected - using legacy test configuration")
            add_definitions(-DDEBIAN9_BUILD)
        elseif(${DEBIAN_VERSION} MATCHES "12.*" OR ${DEBIAN_VERSION} MATCHES "13.*")
            message(STATUS "Debian 12/13 detected - using modern test configuration")
            add_definitions(-DDEBIAN12_BUILD)
        endif()
    endif()
    
    # Add libm for math functions
    target_link_libraries(${TEST_EXECUTABLE} m)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    # macOS-specific test configurations
    message(STATUS "Platform: macOS")
    
    # Add macOS frameworks if needed
    find_library(FOUNDATION Foundation)
    find_library(COREFOUNDATION CoreFoundation)
    if(FOUNDATION AND COREFOUNDATION)
        target_link_libraries(${TEST_EXECUTABLE} ${FOUNDATION} ${COREFOUNDATION})
    endif()
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    # Windows-specific test configurations
    message(STATUS "Platform: Windows")
    
    # Add Windows specific libraries
    target_link_libraries(${TEST_EXECUTABLE} ws2_32)
else()
    message(WARNING "Unsupported platform for tests: ${CMAKE_SYSTEM_NAME}")
endif()

# Add compile definitions for test mode
target_compile_definitions(${TEST_EXECUTABLE} PRIVATE TEST_MODE=1)

# Set output directory for test results
set(TEST_RESULTS_DIR ${CMAKE_CURRENT_BINARY_DIR}/test_results)
file(MAKE_DIRECTORY ${TEST_RESULTS_DIR})

# Add custom command to generate test summary
add_custom_command(TARGET ${TEST_EXECUTABLE} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Test build completed at: $<TARGET_FILE:${TEST_EXECUTABLE}>"
    COMMAND ${CMAKE_COMMAND} -E echo "Test results will be saved to: ${TEST_RESULTS_DIR}"
)

# Add installation target for tests
install(TARGETS ${TEST_EXECUTABLE}
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/tests
    CONFIGURATIONS Debug RelWithDebInfo
)

# Add test data installation
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/test_data
    DESTINATION ${CMAKE_INSTALL_PREFIX}/tests
    CONFIGURATIONS Debug RelWithDebInfo
)

# Add uninstall target for tests
add_custom_target(uninstall_tests
    COMMAND rm -f ${CMAKE_INSTALL_PREFIX}/tests/${TEST_EXECUTABLE}
    COMMAND rm -rf ${CMAKE_INSTALL_PREFIX}/tests/test_data
    COMMENT "Uninstalling test files..."
)