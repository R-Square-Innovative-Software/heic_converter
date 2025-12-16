// image_processor.cpp
// HEIC/HEIF Converter - Image Processing Implementation
// R Square Innovation Software
// Version: v1.0

// image_processor.cpp - Fix constructor and method calls
// HEIC/HEIF Converter - Image Processing Implementation
// R Square Innovation Software
// Version: v1.0

#include "image_processor.h"
#include <string>
#include <vector>
#include <algorithm>
#include <cstring>
#include <filesystem>

// External Includes (System Libraries)
#include <png.h>    // In system library
#include <jpeglib.h> // In system library
#include <webp/encode.h> // In system library

// Local Includes
#include "heic_decoder.h" // In heic_decoder.h
#include "config.h" // In config.h
#include "file_utils.h" // Add this include

ImageProcessor::ImageProcessor(oLogger* pLogger) 
{ // Start Constructor
    m_pLogger = pLogger; // End Assignment
    m_sLastError = ""; // End Assignment
    m_iOutputQuality = 85; // Use hardcoded value instead of DEFAULT_QUALITY
    m_bCodecsInitialized = false; // End Assignment
    m_pHeifContext = nullptr; // End Assignment
    m_pHeifImage = nullptr; // End Assignment
    
    // Initialize codecs
    bool bInitSuccess = fn_initializeCodecs(); // End Function Call
    if (!bInitSuccess) 
    { // Start Condition
        if (m_pLogger) {
            m_pLogger->fn_logError("Failed to initialize codecs"); // In logger.cpp
        }
    } // End Condition(bInitSuccess)
    
} // End Constructor

// Destructor
ImageProcessor::~ImageProcessor() 
{ // Start Destructor
    bool bCleanupSuccess = fn_cleanupResources(); // End Function Call
    if (!bCleanupSuccess && m_pLogger) 
    { // Start Condition
        m_pLogger->fn_logWarning("Failed to cleanup all resources"); // In logger.cpp
    } // End Condition(bCleanupSuccess)
    
    m_pLogger = nullptr; // End Assignment
} // End Destructor

// Public Method: Convert Image
bool ImageProcessor::fn_convertImage(const std::string& sInputPath, 
                                     const std::string& sOutputPath, 
                                     const std::string& sOutputFormat, 
                                     int iQuality) 
{ // Start Function fn_convertImage
    
    // Validate input file exists
    if (!std::filesystem::exists(sInputPath)) 
    { // Start Condition
        m_sLastError = "Input file does not exist: " + sInputPath; // End Assignment
        m_pLogger->fn_logError(m_sLastError); // In logger.cpp
        return false; // End Return
    } // End Condition(file exists)
    
    // Validate output format
    std::string sFormat = sOutputFormat; // End Assignment
    if (sFormat.empty()) 
    { // Start Condition
        sFormat = fn_determineOutputFormat(sOutputPath); // End Function Call
    } // End Condition(sFormat.empty())
    
    bool bValidFormat = fn_validateOutputFormat(sFormat); // End Function Call
    if (!bValidFormat) 
    { // Start Condition
        m_sLastError = "Unsupported output format: " + sFormat; // End Assignment
        m_pLogger->fn_logError(m_sLastError); // In logger.cpp
        return false; // End Return
    } // End Condition(bValidFormat)
    
    // Validate quality parameter
    if (iQuality < 1 || iQuality > 100) 
    { // Start Condition
        m_sLastError = "Quality must be between 1 and 100"; // End Assignment
        m_pLogger->fn_logError(m_sLastError); // In logger.cpp
        return false; // End Return
    } // End Condition(iQuality range)
    
    // Decode HEIC/HEIF image
    unsigned char* pImageData = nullptr; // End Assignment
    int iWidth = 0; // End Assignment
    int iHeight = 0; // End Assignment
    int iChannels = 0; // End Assignment
    
    m_pLogger->fn_logInfo("Decoding image: " + sInputPath); // In logger.cpp
    bool bDecodeSuccess = fn_decodeHEIC(sInputPath, &pImageData, iWidth, iHeight, iChannels); // End Function Call
    if (!bDecodeSuccess) 
    { // Start Condition
        m_sLastError = "Failed to decode image: " + sInputPath; // End Assignment
        m_pLogger->fn_logError(m_sLastError); // In logger.cpp
        return false; // End Return
    } // End Condition(bDecodeSuccess)
    
    // Encode to output format
    m_pLogger->fn_logInfo("Encoding image to: " + sOutputPath); // In logger.cpp
    bool bEncodeSuccess = fn_encodeImage(pImageData, iWidth, iHeight, iChannels, 
                                         sOutputPath, sFormat, iQuality); // End Function Call
    
    // Clean up image data
    if (pImageData != nullptr) 
    { // Start Condition
        delete[] pImageData; // End Memory Free
        pImageData = nullptr; // End Assignment
    } // End Condition(pImageData != nullptr)
    
    if (!bEncodeSuccess) 
    { // Start Condition
        m_sLastError = "Failed to encode image: " + sOutputPath; // End Assignment
        m_pLogger->fn_logError(m_sLastError); // In logger.cpp
        return false; // End Return
    } // End Condition(bEncodeSuccess)
    
    m_pLogger->fn_logInfo("Successfully converted: " + sInputPath + " to " + sOutputPath); // In logger.cpp
    return true; // End Return
    
} // End Function fn_convertImage

// Public Method: Validate Image
bool ImageProcessor::fn_validateImage(const std::string& sImagePath) 
{ // Start Function fn_validateImage
    
    if (!std::filesystem::exists(sImagePath)) 
    { // Start Condition
        m_sLastError = "File does not exist: " + sImagePath; // End Assignment
        return false; // End Return
    } // End Condition(file exists)
    
    // Check file extension for HEIC/HEIF
    std::string sExtension = std::filesystem::path(sImagePath).extension().string(); // End Assignment
    std::transform(sExtension.begin(), sExtension.end(), sExtension.begin(), ::tolower); // End Function Call
    
    std::vector<std::string> vsSupported = fn_getSupportedInputFormats(); // End Function Call
    for (const auto& sFormat : vsSupported) 
    { // Start Loop
        if (sExtension == "." + sFormat) 
        { // Start Condition
            return true; // End Return
        } // End Condition(sExtension == "." + sFormat)
    } // End Loop
    
    m_sLastError = "Unsupported image format: " + sExtension; // End Assignment
    return false; // End Return
    
} // End Function fn_validateImage

// Public Method: Get Supported Input Formats
std::vector<std::string> ImageProcessor::fn_getSupportedInputFormats() 
{ // Start Function fn_getSupportedInputFormats
    
    std::vector<std::string> vsFormats; // End Variable Declaration
    vsFormats.push_back("heic"); // End Function Call
    vsFormats.push_back("heif"); // End Function Call
    vsFormats.push_back("heics"); // End Function Call
    vsFormats.push_back("heifs"); // End Function Call
    
    return vsFormats; // End Return
    
} // End Function fn_getSupportedInputFormats

// Public Method: Get Supported Output Formats
std::vector<std::string> ImageProcessor::fn_getSupportedOutputFormats() 
{ // Start Function fn_getSupportedOutputFormats
    
    std::vector<std::string> vsFormats; // End Variable Declaration
    vsFormats.push_back("jpg"); // End Function Call
    vsFormats.push_back("jpeg"); // End Function Call
    vsFormats.push_back("png"); // End Function Call
    vsFormats.push_back("webp"); // End Function Call
    vsFormats.push_back("bmp"); // End Function Call
    vsFormats.push_back("tiff"); // End Function Call
    
    return vsFormats; // End Return
    
} // End Function fn_getSupportedOutputFormats

// Public Method: Set Output Quality
bool ImageProcessor::fn_setOutputQuality(int iQuality) 
{ // Start Function fn_setOutputQuality
    
    if (iQuality < 1 || iQuality > 100) 
    { // Start Condition
        m_sLastError = "Quality must be between 1 and 100"; // End Assignment
        return false; // End Return
    } // End Condition(iQuality range)
    
    m_iOutputQuality = iQuality; // End Assignment
    return true; // End Return
    
} // End Function fn_setOutputQuality

// Public Method: Get Output Quality
int ImageProcessor::fn_getOutputQuality() 
{ // Start Function fn_getOutputQuality
    return m_iOutputQuality; // End Return
} // End Function fn_getOutputQuality

// Public Method: Get Last Error
std::string ImageProcessor::fn_getLastError() 
{ // Start Function fn_getLastError
    return m_sLastError; // End Return
} // End Function fn_getLastError

// Private Method: Initialize Codecs
bool ImageProcessor::fn_initializeCodecs() 
{ // Start Function fn_initializeCodecs
    
    if (m_bCodecsInitialized) 
    { // Start Condition
        return true; // End Return
    } // End Condition(m_bCodecsInitialized)
    
    // Initialize codecs - this will happen automatically when needed
    // The actual decoder initialization happens in fn_decodeHEIC
    m_bCodecsInitialized = true; // Set to true for now
    
    if (m_pLogger) {
        m_pLogger->fn_logInfo("Codecs marked as initialized");
    }
    
    return true; // End Return
    
} // End Function fn_initializeCodecs

// Private Method: Cleanup Resources
bool ImageProcessor::fn_cleanupResources() 
{ // Start Function fn_cleanupResources
    
    bool bSuccess = true; // End Assignment
    
    // Cleanup HEIC decoder resources
    if (m_pHeifContext != nullptr) 
    { // Start Condition
        // Cleanup code will be in heic_decoder.cpp
        // This is a placeholder
        m_pHeifContext = nullptr; // End Assignment
    } // End Condition(m_pHeifContext != nullptr)
    
    if (m_pHeifImage != nullptr) 
    { // Start Condition
        // Cleanup code will be in heic_decoder.cpp
        m_pHeifImage = nullptr; // End Assignment
    } // End Condition(m_pHeifImage != nullptr)
    
    m_bCodecsInitialized = false; // End Assignment
    return bSuccess; // End Return
    
} // End Function fn_cleanupResources

// Private Method: Decode HEIC
bool ImageProcessor::fn_decodeHEIC(const std::string& sInputPath, 
                                  unsigned char** ppImageData, 
                                  int& iWidth, 
                                  int& iHeight, 
                                  int& iChannels) 
{ // Start Function fn_decodeHEIC
    
    // FIXED: Use fn_decodeFile instead of fn_decodeImage
    HeicDecoder oDecoder; // End Variable Declaration
    
    // Set the embedded codec path based on build configuration
    std::string sCodecPath;
    #ifdef DEBIAN9_BUILD
        sCodecPath = "../data/debian9/";
    #elif defined(DEBIAN12_BUILD)
        sCodecPath = "../data/debian12/";
    #else
        sCodecPath = "../data/";
    #endif
    
    oDecoder.fn_setEmbeddedCodecPath(sCodecPath);
    
    oDecodedImage oResult = oDecoder.fn_decodeFile(sInputPath); // Use fn_decodeFile
    
    if (!oResult.sError.empty()) 
    { // Start Condition
        m_sLastError = oResult.sError; // In heic_decoder.cpp
        return false;
    } // End Condition(!oResult.sError.empty())
    
    // Allocate memory and copy data
    iWidth = oResult.iWidth;
    iHeight = oResult.iHeight;
    iChannels = oResult.iChannels;
    
    size_t dataSize = oResult.vData.size();
    *ppImageData = new unsigned char[dataSize];
    std::copy(oResult.vData.begin(), oResult.vData.end(), *ppImageData);
    
    return true; // End Return
    
} // End Function fn_decodeHEIC

// Private Method: Encode Image
bool ImageProcessor::fn_encodeImage(const unsigned char* pImageData, 
                                   int iWidth, 
                                   int iHeight, 
                                   int iChannels, 
                                   const std::string& sOutputPath, 
                                   const std::string& sOutputFormat, 
                                   int iQuality) 
{ // Start Function fn_encodeImage
    
    std::string sFormat = sOutputFormat; // End Assignment
    std::transform(sFormat.begin(), sFormat.end(), sFormat.begin(), ::tolower); // End Function Call
    
    // Dispatch to appropriate encoder based on format
    if (sFormat == "jpg" || sFormat == "jpeg") 
    { // Start Condition
        // JPEG encoding will be implemented in format_encoder.cpp
        // Placeholder for now
        return false; // End Return
    } 
    else if (sFormat == "png") 
    { // Start Condition
        // PNG encoding will be implemented in format_encoder.cpp
        // Placeholder for now
        return false; // End Return
    } 
    else if (sFormat == "webp") 
    { // Start Condition
        // WebP encoding will be implemented in format_encoder.cpp
        // Placeholder for now
        return false; // End Return
    } 
    else if (sFormat == "bmp") 
    { // Start Condition
        // BMP encoding will be implemented in format_encoder.cpp
        // Placeholder for now
        return false; // End Return
    } 
    else if (sFormat == "tiff") 
    { // Start Condition
        // TIFF encoding will be implemented in format_encoder.cpp
        // Placeholder for now
        return false; // End Return
    } 
    else 
    { // Start Condition
        m_sLastError = "Unsupported output format for encoding: " + sFormat; // End Assignment
        return false; // End Return
    } // End Condition(sFormat)
    
} // End Function fn_encodeImage

// Private Method: Validate Output Format
bool ImageProcessor::fn_validateOutputFormat(const std::string& sFormat) 
{ // Start Function fn_validateOutputFormat
    
    std::string sLowerFormat = sFormat; // End Assignment
    std::transform(sLowerFormat.begin(), sLowerFormat.end(), sLowerFormat.begin(), ::tolower); // End Function Call
    
    std::vector<std::string> vsSupported = fn_getSupportedOutputFormats(); // End Function Call
    for (const auto& sSupportedFormat : vsSupported) 
    { // Start Loop
        if (sLowerFormat == sSupportedFormat) 
        { // Start Condition
            return true; // End Return
        } // End Condition(sLowerFormat == sSupportedFormat)
    } // End Loop
    
    return false; // End Return
    
} // End Function fn_validateOutputFormat

// Private Method: Determine Output Format from Path
std::string ImageProcessor::fn_determineOutputFormat(const std::string& sOutputPath) 
{ // Start Function fn_determineOutputFormat
    
    std::filesystem::path oPath(sOutputPath); // End Variable Declaration
    std::string sExtension = oPath.extension().string(); // End Assignment
    
    if (sExtension.empty()) 
    { // Start Condition
        return "jpg"; // End Return (default format)
    } // End Condition(sExtension.empty())
    
    // Remove the dot
    if (!sExtension.empty() && sExtension[0] == '.') 
    { // Start Condition
        sExtension = sExtension.substr(1); // End Assignment
    } // End Condition(sExtension[0] == '.')
    
    std::transform(sExtension.begin(), sExtension.end(), sExtension.begin(), ::tolower); // End Function Call
    
    // Validate the format
    bool bValid = fn_validateOutputFormat(sExtension); // End Function Call
    if (!bValid) 
    { // Start Condition
        return "jpg"; // End Return (default format)
    } // End Condition(bValid)
    
    return sExtension; // End Return
    
} // End Function fn_determineOutputFormat