// config.h - Configuration constants and macros for HEIC Converter
// Author: R Square Innovation Software
// Version: v1.0

#ifndef HEIC_CONVERTER_CONFIG_H
#define HEIC_CONVERTER_CONFIG_H

#include <string>
#include <vector>

// Program Information
const std::string sPROGRAM_NAME = "heic_converter";
const std::string sVERSION = "v1.0";
const std::string sAUTHOR = "R Square Innovation Software";

// Build Configuration
#ifdef DEBIAN9_BUILD
const std::string sBUILD_TYPE = "debian9";
#elif defined(DEBIAN12_BUILD)
const std::string sBUILD_TYPE = "debian12";
#else
const std::string sBUILD_TYPE = "generic";
#endif

// Embedded Codec Paths
const std::string sEMBEDDED_CODEC_PATH = "../data/"; // Local Function
const std::string sDEBIAN9_CODEC_PATH = "../data/debian9/"; // Local Function
const std::string sDEBIAN12_CODEC_PATH = "../data/debian12/"; // Local Function

// Default Settings
const int iDEFAULT_JPEG_QUALITY = 85; // Local Function
const int iDEFAULT_PNG_COMPRESSION = 6; // Local Function
const int iDEFAULT_THREAD_COUNT = 4; // Local Function
const int iMAX_THREAD_COUNT = 16; // Local Function
const float fDEFAULT_SCALE_FACTOR = 1.0f; // Local Function
const bool bDEFAULT_OVERWRITE = false; // Local Function
const bool bDEFAULT_VERBOSE = false; // Local Function
const bool bDEFAULT_RECURSIVE = false; // Local Function

// Supported Input Formats
const std::vector<std::string> vsSUPPORTED_INPUT_FORMATS = { // Local Function
    ".heic",
    ".heif",
    ".HEIC",
    ".HEIF"
};

// Supported Output Formats
const std::vector<std::string> vsSUPPORTED_OUTPUT_FORMATS = { // Local Function
    ".jpg",
    ".jpeg",
    ".png",
    ".bmp",
    ".tiff",
    ".webp",
    ".JPG",
    ".JPEG",
    ".PNG",
    ".BMP",
    ".TIFF",
    ".WEBP"
};

// Format MIME Types Mapping
struct oFormatMimeType { // Local Function
    std::string sExtension; // Local Function
    std::string sMimeType; // Local Function
}; // End struct oFormatMimeType

const std::vector<oFormatMimeType> voFORMAT_MIME_TYPES = { // Local Function
    {".jpg", "image/jpeg"}, // Local Function
    {".jpeg", "image/jpeg"}, // Local Function
    {".png", "image/png"}, // Local Function
    {".bmp", "image/bmp"}, // Local Function
    {".tiff", "image/tiff"}, // Local Function
    {".webp", "image/webp"}, // Local Function
    {".heic", "image/heic"}, // Local Function
    {".heif", "image/heif"} // Local Function
}; // End vector voFORMAT_MIME_TYPES

// Error Codes
enum eErrorCode { // Local Function
    ERROR_SUCCESS = 0, // Local Function
    ERROR_INVALID_ARGUMENTS = 1, // Local Function
    ERROR_UNSUPPORTED_FORMAT = 2, // Local Function
    ERROR_FILE_NOT_FOUND = 3, // Local Function
    ERROR_READ_PERMISSION = 4, // Local Function
    ERROR_WRITE_PERMISSION = 5, // Local Function
    ERROR_DECODING_FAILED = 6, // Local Function
    ERROR_ENCODING_FAILED = 7, // Local Function
    ERROR_MEMORY_ALLOCATION = 8, // Local Function
    ERROR_CODEC_INITIALIZATION = 9, // Local Function
    ERROR_BATCH_PROCESSING = 10, // Local Function
    ERROR_UNKNOWN = 255 // Local Function
}; // End enum eErrorCode

// Configuration Structure
struct oConfig { // Local Function
    std::string sInputPath; // Local Function
    std::string sOutputPath; // Local Function
    std::string sOutputFormat; // Local Function
    int iJpegQuality; // Local Function
    int iPngCompression; // Local Function
    int iThreadCount; // Local Function
    float fScaleFactor; // Local Function
    bool bOverwrite; // Local Function
    bool bVerbose; // Local Function
    bool bRecursive; // Local Function
    bool bKeepMetadata; // Local Function
    bool bStripColorProfile; // Local Function
}; // End struct oConfig

// Function Declarations
std::string fn_getDefaultOutputFormat(); // In config.cpp
std::string fn_getDefaultOutputPath(const std::string& sInputPath); // In config.cpp
bool fn_isSupportedInputFormat(const std::string& sExtension); // In config.cpp
bool fn_isSupportedOutputFormat(const std::string& sExtension); // In config.cpp
std::string fn_normalizeExtension(const std::string& sExtension); // In config.cpp
std::string fn_getMimeTypeForExtension(const std::string& sExtension); // In config.cpp
std::string fn_getExtensionForMimeType(const std::string& sMimeType); // In config.cpp
oConfig fn_getDefaultConfig(); // In config.cpp
std::string fn_getEmbeddedCodecPath(); // In config.cpp
void fn_printConfig(const oConfig& oCurrentConfig); // In config.cpp

#endif // HEIC_CONVERTER_CONFIG_H
// End ifndef HEIC_CONVERTER_CONFIG_H