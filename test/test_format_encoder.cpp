// test_format_encoder.cpp - Test file for format encoder functionality
// Author: R Square Innovation Software
// Version: v1.0

#include "format_encoder.h"
#include "config.h"
#include "logger.h"
#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <cstdlib>
#include <memory>
#include <chrono>

// Test data structure
struct oTestImageData 
{ // Begin struct oTestImageData
    int iWidth; // Local Function
    int iHeight; // Local Function
    int iChannels; // Local Function
    std::vector<unsigned char> vImageData; // Local Function
}; // End struct oTestImageData

// Local Function
oTestImageData fn_createTestImage(int iWidth, int iHeight, int iChannels) 
{ // Begin fn_createTestImage
    oTestImageData oTestData; // Local Function
    oTestData.iWidth = iWidth; // Local Function
    oTestData.iHeight = iHeight; // Local Function
    oTestData.iChannels = iChannels; // Local Function
    
    // Create a simple gradient test image
    size_t iImageSize = iWidth * iHeight * iChannels; // Local Function
    oTestData.vImageData.resize(iImageSize); // Local Function
    
    for (int y = 0; y < iHeight; y++) 
    { // Begin for
        for (int x = 0; x < iWidth; x++) 
        { // Begin for
            int iIndex = (y * iWidth + x) * iChannels; // Local Function
            
            // Create gradient: red increases horizontally, green increases vertically
            oTestData.vImageData[iIndex] = static_cast<unsigned char>((255.0f * x) / iWidth); // Red // Local Function
            
            if (iChannels > 1) 
            { // Begin if
                oTestData.vImageData[iIndex + 1] = static_cast<unsigned char>((255.0f * y) / iHeight); // Green // Local Function
            } // End if(iChannels > 1)
            
            if (iChannels > 2) 
            { // Begin if
                oTestData.vImageData[iIndex + 2] = 128; // Blue // Local Function
            } // End if(iChannels > 2)
            
            if (iChannels > 3) 
            { // Begin if
                oTestData.vImageData[iIndex + 3] = 255; // Alpha (fully opaque) // Local Function
            } // End if(iChannels > 3)
        } // End for(int x = 0; x < iWidth; x++)
    } // End for(int y = 0; y < iHeight; y++)
    
    return oTestData; // Return test image data
} // End Function fn_createTestImage

// Local Function
bool fn_cleanupTestFile(const std::string& sFilePath) 
{ // Begin fn_cleanupTestFile
    if (std::remove(sFilePath.c_str()) == 0) 
    { // Begin if
        std::cout << "  Cleaned up test file: " << sFilePath << std::endl; // In iostream
        return true; // File removed successfully
    } // End if(std::remove(sFilePath.c_str()) == 0)
    
    std::cerr << "  Warning: Failed to clean up test file: " << sFilePath << std::endl; // In iostream
    return false; // File removal failed
} // End Function fn_cleanupTestFile

// Local Function
bool fn_verifyTestFile(const std::string& sFilePath, size_t iMinSize = 100) 
{ // Begin fn_verifyTestFile
    // Open file for reading
    FILE* pFile = fopen(sFilePath.c_str(), "rb"); // Local Function
    if (!pFile) 
    { // Begin if
        std::cerr << "  Error: Could not open file for verification: " << sFilePath << std::endl; // In iostream
        return false; // File cannot be opened
    } // End if(!pFile)
    
    // Get file size
    fseek(pFile, 0, SEEK_END); // Local Function
    long iFileSize = ftell(pFile); // Local Function
    fclose(pFile); // Local Function
    
    // Verify minimum size
    if (iFileSize < static_cast<long>(iMinSize)) 
    { // Begin if
        std::cerr << "  Error: File size too small: " << iFileSize << " bytes (minimum: " << iMinSize << ")" << std::endl; // In iostream
        return false; // File too small
    } // End if(iFileSize < static_cast<long>(iMinSize))
    
    std::cout << "  File verification passed: " << sFilePath << " (" << iFileSize << " bytes)" << std::endl; // In iostream
    return true; // File verification passed
} // End Function fn_verifyTestFile

// Local Function
bool fn_testJPEGEncoding() 
{ // Begin fn_testJPEGEncoding
    std::cout << "Testing JPEG encoding..." << std::endl; // In iostream
    
    // Create test configuration
    oConfig oTestConfig = fn_getDefaultConfig(); // In config.cpp
    oTestConfig.sOutputFormat = ".jpg"; // Local Function
    oTestConfig.iJpegQuality = 90; // Local Function
    
    // Create test image
    oTestImageData oTestImage = fn_createTestImage(640, 480, 3); // Local Function
    
    // Create format encoder
    oFormatEncoder oEncoder; // In format_encoder.h
    int iInitResult = oEncoder.fn_initialize(oTestConfig); // In format_encoder.cpp
    
    if (iInitResult != ERROR_SUCCESS) 
    { // Begin if
        std::cerr << "  Error: Failed to initialize JPEG encoder" << std::endl; // In iostream
        return false; // Initialization failed
    } // End if(iInitResult != ERROR_SUCCESS)
    
    // Test file path
    std::string sTestFilePath = "test_jpeg_output.jpg"; // Local Function
    
    // Encode image
    auto tStartTime = std::chrono::high_resolution_clock::now(); // In chrono
    
    int iEncodeResult = oEncoder.fn_encodeImage( // In format_encoder.cpp
        sTestFilePath, // Local Function
        oTestImage.vImageData.data(), // Local Function
        oTestImage.iWidth, // Local Function
        oTestImage.iHeight, // Local Function
        oTestImage.iChannels // Local Function
    );
    
    auto tEndTime = std::chrono::high_resolution_clock::now(); // In chrono
    auto tDuration = std::chrono::duration_cast<std::chrono::milliseconds>(tEndTime - tStartTime); // In chrono
    
    if (iEncodeResult != ERROR_SUCCESS) 
    { // Begin if
        std::cerr << "  Error: JPEG encoding failed with code: " << iEncodeResult << std::endl; // In iostream
        return false; // Encoding failed
    } // End if(iEncodeResult != ERROR_SUCCESS)
    
    std::cout << "  JPEG encoding completed in " << tDuration.count() << " ms" << std::endl; // In iostream
    
    // Verify output file
    bool bVerificationResult = fn_verifyTestFile(sTestFilePath, 1000); // Local Function
    
    // Cleanup
    bool bCleanupResult = fn_cleanupTestFile(sTestFilePath); // Local Function
    
    return bVerificationResult; // Return verification result
} // End Function fn_testJPEGEncoding

// Local Function
bool fn_testPNGEncoding() 
{ // Begin fn_testPNGEncoding
    std::cout << "Testing PNG encoding..." << std::endl; // In iostream
    
    // Create test configuration
    oConfig oTestConfig = fn_getDefaultConfig(); // In config.cpp
    oTestConfig.sOutputFormat = ".png"; // Local Function
    oTestConfig.iPngCompression = 6; // Local Function
    
    // Create test image with alpha channel
    oTestImageData oTestImage = fn_createTestImage(800, 600, 4); // Local Function
    
    // Create format encoder
    oFormatEncoder oEncoder; // In format_encoder.h
    int iInitResult = oEncoder.fn_initialize(oTestConfig); // In format_encoder.cpp
    
    if (iInitResult != ERROR_SUCCESS) 
    { // Begin if
        std::cerr << "  Error: Failed to initialize PNG encoder" << std::endl; // In iostream
        return false; // Initialization failed
    } // End if(iInitResult != ERROR_SUCCESS)
    
    // Test file path
    std::string sTestFilePath = "test_png_output.png"; // Local Function
    
    // Encode image
    auto tStartTime = std::chrono::high_resolution_clock::now(); // In chrono
    
    int iEncodeResult = oEncoder.fn_encodeImage( // In format_encoder.cpp
        sTestFilePath, // Local Function
        oTestImage.vImageData.data(), // Local Function
        oTestImage.iWidth, // Local Function
        oTestImage.iHeight, // Local Function
        oTestImage.iChannels // Local Function
    );
    
    auto tEndTime = std::chrono::high_resolution_clock::now(); // In chrono
    auto tDuration = std::chrono::duration_cast<std::chrono::milliseconds>(tEndTime - tStartTime); // In chrono
    
    if (iEncodeResult != ERROR_SUCCESS) 
    { // Begin if
        std::cerr << "  Error: PNG encoding failed with code: " << iEncodeResult << std::endl; // In iostream
        return false; // Encoding failed
    } // End if(iEncodeResult != ERROR_SUCCESS)
    
    std::cout << "  PNG encoding completed in " << tDuration.count() << " ms" << std::endl; // In iostream
    
    // Verify output file
    bool bVerificationResult = fn_verifyTestFile(sTestFilePath, 2000); // Local Function
    
    // Cleanup
    bool bCleanupResult = fn_cleanupTestFile(sTestFilePath); // Local Function
    
    return bVerificationResult; // Return verification result
} // End Function fn_testPNGEncoding

// Local Function
bool fn_testBMPEncoding() 
{ // Begin fn_testBMPEncoding
    std::cout << "Testing BMP encoding..." << std::endl; // In iostream
    
    // Create test configuration
    oConfig oTestConfig = fn_getDefaultConfig(); // In config.cpp
    oTestConfig.sOutputFormat = ".bmp"; // Local Function
    
    // Create test image
    oTestImageData oTestImage = fn_createTestImage(320, 240, 3); // Local Function
    
    // Create format encoder
    oFormatEncoder oEncoder; // In format_encoder.h
    int iInitResult = oEncoder.fn_initialize(oTestConfig); // In format_encoder.cpp
    
    if (iInitResult != ERROR_SUCCESS) 
    { // Begin if
        std::cerr << "  Error: Failed to initialize BMP encoder" << std::endl; // In iostream
        return false; // Initialization failed
    } // End if(iInitResult != ERROR_SUCCESS)
    
    // Test file path
    std::string sTestFilePath = "test_bmp_output.bmp"; // Local Function
    
    // Encode image
    auto tStartTime = std::chrono::high_resolution_clock::now(); // In chrono
    
    int iEncodeResult = oEncoder.fn_encodeImage( // In format_encoder.cpp
        sTestFilePath, // Local Function
        oTestImage.vImageData.data(), // Local Function
        oTestImage.iWidth, // Local Function
        oTestImage.iHeight, // Local Function
        oTestImage.iChannels // Local Function
    );
    
    auto tEndTime = std::chrono::high_resolution_clock::now(); // In chrono
    auto tDuration = std::chrono::duration_cast<std::chrono::milliseconds>(tEndTime - tStartTime); // In chrono
    
    if (iEncodeResult != ERROR_SUCCESS) 
    { // Begin if
        std::cerr << "  Error: BMP encoding failed with code: " << iEncodeResult << std::endl; // In iostream
        return false; // Encoding failed
    } // End if(iEncodeResult != ERROR_SUCCESS)
    
    std::cout << "  BMP encoding completed in " << tDuration.count() << " ms" << std::endl; // In iostream
    
    // Verify output file
    bool bVerificationResult = fn_verifyTestFile(sTestFilePath, 1000); // Local Function
    
    // Cleanup
    bool bCleanupResult = fn_cleanupTestFile(sTestFilePath); // Local Function
    
    return bVerificationResult; // Return verification result
} // End Function fn_testBMPEncoding

// Local Function
bool fn_testTIFFEncoding() 
{ // Begin fn_testTIFFEncoding
    std::cout << "Testing TIFF encoding..." << std::endl; // In iostream
    
    // Create test configuration
    oConfig oTestConfig = fn_getDefaultConfig(); // In config.cpp
    oTestConfig.sOutputFormat = ".tiff"; // Local Function
    
    // Create test image
    oTestImageData oTestImage = fn_createTestImage(512, 512, 3); // Local Function
    
    // Create format encoder
    oFormatEncoder oEncoder; // In format_encoder.h
    int iInitResult = oEncoder.fn_initialize(oTestConfig); // In format_encoder.cpp
    
    if (iInitResult != ERROR_SUCCESS) 
    { // Begin if
        std::cerr << "  Error: Failed to initialize TIFF encoder" << std::endl; // In iostream
        return false; // Initialization failed
    } // End if(iInitResult != ERROR_SUCCESS)
    
    // Test file path
    std::string sTestFilePath = "test_tiff_output.tiff"; // Local Function
    
    // Encode image
    auto tStartTime = std::chrono::high_resolution_clock::now(); // In chrono
    
    int iEncodeResult = oEncoder.fn_encodeImage( // In format_encoder.cpp
        sTestFilePath, // Local Function
        oTestImage.vImageData.data(), // Local Function
        oTestImage.iWidth, // Local Function
        oTestImage.iHeight, // Local Function
        oTestImage.iChannels // Local Function
    );
    
    auto tEndTime = std::chrono::high_resolution_clock::now(); // In chrono
    auto tDuration = std::chrono::duration_cast<std::chrono::milliseconds>(tEndTime - tStartTime); // In chrono
    
    if (iEncodeResult != ERROR_SUCCESS) 
    { // Begin if
        std::cerr << "  Error: TIFF encoding failed with code: " << iEncodeResult << std::endl; // In iostream
        return false; // Encoding failed
    } // End if(iEncodeResult != ERROR_SUCCESS)
    
    std::cout << "  TIFF encoding completed in " << tDuration.count() << " ms" << std::endl; // In iostream
    
    // Verify output file
    bool bVerificationResult = fn_verifyTestFile(sTestFilePath, 2000); // Local Function
    
    // Cleanup
    bool bCleanupResult = fn_cleanupTestFile(sTestFilePath); // Local Function
    
    return bVerificationResult; // Return verification result
} // End Function fn_testTIFFEncoding

// Local Function
bool fn_testWebPEncoding() 
{ // Begin fn_testWebPEncoding
    std::cout << "Testing WebP encoding..." << std::endl; // In iostream
    
    // Create test configuration
    oConfig oTestConfig = fn_getDefaultConfig(); // In config.cpp
    oTestConfig.sOutputFormat = ".webp"; // Local Function
    
    // Create test image
    oTestImageData oTestImage = fn_createTestImage(1024, 768, 3); // Local Function
    
    // Create format encoder
    oFormatEncoder oEncoder; // In format_encoder.h
    int iInitResult = oEncoder.fn_initialize(oTestConfig); // In format_encoder.cpp
    
    if (iInitResult != ERROR_SUCCESS) 
    { // Begin if
        std::cerr << "  Error: Failed to initialize WebP encoder" << std::endl; // In iostream
        return false; // Initialization failed
    } // End if(iInitResult != ERROR_SUCCESS)
    
    // Test file path
    std::string sTestFilePath = "test_webp_output.webp"; // Local Function
    
    // Encode image
    auto tStartTime = std::chrono::high_resolution_clock::now(); // In chrono
    
    int iEncodeResult = oEncoder.fn_encodeImage( // In format_encoder.cpp
        sTestFilePath, // Local Function
        oTestImage.vImageData.data(), // Local Function
        oTestImage.iWidth, // Local Function
        oTestImage.iHeight, // Local Function
        oTestImage.iChannels // Local Function
    );
    
    auto tEndTime = std::chrono::high_resolution_clock::now(); // In chrono
    auto tDuration = std::chrono::duration_cast<std::chrono::milliseconds>(tEndTime - tStartTime); // In chrono
    
    if (iEncodeResult != ERROR_SUCCESS) 
    { // Begin if
        std::cerr << "  Error: WebP encoding failed with code: " << iEncodeResult << std::endl; // In iostream
        return false; // Encoding failed
    } // End if(iEncodeResult != ERROR_SUCCESS)
    
    std::cout << "  WebP encoding completed in " << tDuration.count() << " ms" << std::endl; // In iostream
    
    // Verify output file
    bool bVerificationResult = fn_verifyTestFile(sTestFilePath, 500); // Local Function
    
    // Cleanup
    bool bCleanupResult = fn_cleanupTestFile(sTestFilePath); // Local Function
    
    return bVerificationResult; // Return verification result
} // End Function fn_testWebPEncoding

// Local Function
bool fn_testQualitySettings() 
{ // Begin fn_testQualitySettings
    std::cout << "Testing JPEG quality settings..." << std::endl; // In iostream
    
    // Create test image
    oTestImageData oTestImage = fn_createTestImage(400, 300, 3); // Local Function
    
    // Test different quality settings
    std::vector<int> viQualityLevels = {10, 50, 90}; // Local Function
    std::vector<size_t> viExpectedSizes = {5000, 15000, 30000}; // Local Function
    
    bool bAllTestsPassed = true; // Local Function
    
    for (size_t i = 0; i < viQualityLevels.size(); i++) 
    { // Begin for
        int iQuality = viQualityLevels[i]; // Local Function
        
        // Create test configuration
        oConfig oTestConfig = fn_getDefaultConfig(); // In config.cpp
        oTestConfig.sOutputFormat = ".jpg"; // Local Function
        oTestConfig.iJpegQuality = iQuality; // Local Function
        
        // Create format encoder
        oFormatEncoder oEncoder; // In format_encoder.h
        int iInitResult = oEncoder.fn_initialize(oTestConfig); // In format_encoder.cpp
        
        if (iInitResult != ERROR_SUCCESS) 
        { // Begin if
            std::cerr << "  Error: Failed to initialize encoder for quality " << iQuality << std::endl; // In iostream
            bAllTestsPassed = false; // Local Function
            continue; // Skip this test
        } // End if(iInitResult != ERROR_SUCCESS)
        
        // Test file path
        std::string sTestFilePath = "test_quality_" + std::to_string(iQuality) + ".jpg"; // Local Function
        
        // Encode image
        int iEncodeResult = oEncoder.fn_encodeImage( // In format_encoder.cpp
            sTestFilePath, // Local Function
            oTestImage.vImageData.data(), // Local Function
            oTestImage.iWidth, // Local Function
            oTestImage.iHeight, // Local Function
            oTestImage.iChannels // Local Function
        );
        
        if (iEncodeResult != ERROR_SUCCESS) 
        { // Begin if
            std::cerr << "  Error: Encoding failed for quality " << iQuality << std::endl; // In iostream
            bAllTestsPassed = false; // Local Function
            continue; // Skip cleanup and move to next test
        } // End if(iEncodeResult != ERROR_SUCCESS)
        
        // Verify output file
        if (!fn_verifyTestFile(sTestFilePath, 100)) 
        { // Begin if
            bAllTestsPassed = false; // Local Function
        } // End if(!fn_verifyTestFile(sTestFilePath, 100))
        
        // Cleanup
        fn_cleanupTestFile(sTestFilePath); // Local Function
    } // End for(size_t i = 0; i < viQualityLevels.size(); i++)
    
    return bAllTestsPassed; // Return overall test result
} // End Function fn_testQualitySettings

// Local Function
bool fn_testInvalidInput() 
{ // Begin fn_testInvalidInput
    std::cout << "Testing invalid input handling..." << std::endl; // In iostream
    
    // Create test configuration
    oConfig oTestConfig = fn_getDefaultConfig(); // In config.cpp
    oTestConfig.sOutputFormat = ".jpg"; // Local Function
    
    // Create format encoder
    oFormatEncoder oEncoder; // In format_encoder.h
    int iInitResult = oEncoder.fn_initialize(oTestConfig); // In format_encoder.cpp
    
    if (iInitResult != ERROR_SUCCESS) 
    { // Begin if
        std::cerr << "  Error: Failed to initialize encoder" << std::endl; // In iostream
        return false; // Initialization failed
    } // End if(iInitResult != ERROR_SUCCESS)
    
    // Test with null data
    std::string sTestFilePath = "test_invalid_output.jpg"; // Local Function
    
    int iEncodeResult = oEncoder.fn_encodeImage( // In format_encoder.cpp
        sTestFilePath, // Local Function
        nullptr, // Null data // Local Function
        100, // Local Function
        100, // Local Function
        3 // Local Function
    );
    
    if (iEncodeResult == ERROR_SUCCESS) 
    { // Begin if
        std::cerr << "  Error: Encoding should have failed with null data" << std::endl; // In iostream
        fn_cleanupTestFile(sTestFilePath); // Local Function
        return false; // Test failed
    } // End if(iEncodeResult == ERROR_SUCCESS)
    
    std::cout << "  Correctly rejected null data (expected)" << std::endl; // In iostream
    
    // Test with invalid dimensions
    oTestImageData oTestImage = fn_createTestImage(10, 10, 3); // Local Function
    
    iEncodeResult = oEncoder.fn_encodeImage( // In format_encoder.cpp
        sTestFilePath, // Local Function
        oTestImage.vImageData.data(), // Local Function
        0, // Invalid width // Local Function
        100, // Local Function
        3 // Local Function
    );
    
    if (iEncodeResult == ERROR_SUCCESS) 
    { // Begin if
        std::cerr << "  Error: Encoding should have failed with invalid width" << std::endl; // In iostream
        fn_cleanupTestFile(sTestFilePath); // Local Function
        return false; // Test failed
    } // End if(iEncodeResult == ERROR_SUCCESS)
    
    std::cout << "  Correctly rejected invalid dimensions (expected)" << std::endl; // In iostream
    
    return true; // All invalid input tests passed
} // End Function fn_testInvalidInput

// Local Function
int main() 
{ // Begin main
    std::cout << "========================================" << std::endl; // In iostream
    std::cout << "Format Encoder Test Suite" << std::endl; // In iostream
    std::cout << "Version: " << sVERSION << std::endl; // In iostream
    std::cout << "========================================" << std::endl; // In iostream
    std::cout << std::endl; // In iostream
    
    // Test counter
    int iTestsPassed = 0; // Local Function
    int iTestsFailed = 0; // Local Function
    int iTotalTests = 0; // Local Function
    
    // Run tests
    auto fn_runTest = [&](const std::string& sTestName, bool (*pTestFunction)()) 
    { // Begin lambda
        iTotalTests++; // Local Function
        std::cout << std::endl << "Test " << iTotalTests << ": " << sTestName << std::endl; // In iostream
        
        try 
        { // Begin try
            bool bResult = pTestFunction(); // Local Function
            if (bResult) 
            { // Begin if
                std::cout << "  ✓ PASS" << std::endl; // In iostream
                iTestsPassed++; // Local Function
            } 
            else 
            { // Begin else
                std::cout << "  ✗ FAIL" << std::endl; // In iostream
                iTestsFailed++; // Local Function
            } // End else
        } 
        catch (const std::exception& e) 
        { // Begin catch
            std::cerr << "  ✗ EXCEPTION: " << e.what() << std::endl; // In iostream
            iTestsFailed++; // Local Function
        } // End catch(const std::exception& e)
    }; // End lambda
    
    // Execute tests
    fn_runTest("JPEG Encoding", fn_testJPEGEncoding); // Local Function
    fn_runTest("PNG Encoding", fn_testPNGEncoding); // Local Function
    fn_runTest("BMP Encoding", fn_testBMPEncoding); // Local Function
    fn_runTest("TIFF Encoding", fn_testTIFFEncoding); // Local Function
    fn_runTest("WebP Encoding", fn_testWebPEncoding); // Local Function
    fn_runTest("Quality Settings", fn_testQualitySettings); // Local Function
    fn_runTest("Invalid Input Handling", fn_testInvalidInput); // Local Function
    
    // Summary
    std::cout << std::endl << "========================================" << std::endl; // In iostream
    std::cout << "Test Summary:" << std::endl; // In iostream
    std::cout << "  Total tests: " << iTotalTests << std::endl; // In iostream
    std::cout << "  Passed: " << iTestsPassed << std::endl; // In iostream
    std::cout << "  Failed: " << iTestsFailed << std::endl; // In iostream
    
    if (iTestsFailed == 0) 
    { // Begin if
        std::cout << "  Status: ALL TESTS PASSED" << std::endl; // In iostream
        std::cout << "========================================" << std::endl; // In iostream
        return 0; // Success
    } 
    else 
    { // Begin else
        std::cout << "  Status: " << iTestsFailed << " TEST(S) FAILED" << std::endl; // In iostream
        std::cout << "========================================" << std::endl; // In iostream
        return 1; // Failure
    } // End else
} // End Function main