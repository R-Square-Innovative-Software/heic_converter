// config.cpp - Configuration functions implementation for HEIC Converter
// Author: R Square Innovation Software
// Version: v1.0

#include "config.h"
#include <algorithm>
#include <cctype>
#include <filesystem>
#include <iostream>

// Local Function
std::string fn_getDefaultOutputFormat() 
{ // Begin fn_getDefaultOutputFormat
    return ".jpg"; // Default to JPEG
} // End Function fn_getDefaultOutputFormat

// Local Function
std::string fn_getDefaultOutputPath(const std::string& sInputPath) 
{ // Begin fn_getDefaultOutputPath
    std::filesystem::path oInputPath(sInputPath); // In filesystem
    std::filesystem::path oOutputPath = oInputPath; // In filesystem
    
    // If input is a directory, output to the same directory with converted files
    if (std::filesystem::is_directory(oInputPath)) 
    { // Begin if
        return sInputPath; // Return the same directory
    } // End if(std::filesystem::is_directory(oInputPath))
    
    // For single file, change extension to default format
    if (oInputPath.has_extension()) 
    { // Begin if
        std::string sStem = oInputPath.stem().string(); // In filesystem
        return oInputPath.parent_path().string() + "/" + sStem + fn_getDefaultOutputFormat(); // In filesystem
    } // End if(oInputPath.has_extension())
    
    return sInputPath + fn_getDefaultOutputFormat(); // Append default extension
} // End Function fn_getDefaultOutputPath

// Local Function
bool fn_isSupportedInputFormat(const std::string& sExtension) 
{ // Begin fn_isSupportedInputFormat
    if (sExtension.empty()) {
        return false;
    }
    
    std::string sLowerExtension = sExtension; // Local Function
    std::transform(sLowerExtension.begin(), sLowerExtension.end(), sLowerExtension.begin(), // In algorithm
                   [](unsigned char c) { return std::tolower(c); }); // In cctype
    
    // Add dot for comparison if not present
    if (!sLowerExtension.empty() && sLowerExtension[0] != '.') {
        sLowerExtension = "." + sLowerExtension;
    }
    
    for (const auto& sSupportedFormat : vsSUPPORTED_INPUT_FORMATS) 
    { // Begin for
        if (sLowerExtension == sSupportedFormat) 
        { // Begin if
            return true; // Format is supported
        } // End if(sLowerExtension == sSupportedFormat)
    } // End for(const auto& sSupportedFormat : vsSUPPORTED_INPUT_FORMATS)
    
    return false; // Format not supported
} // End Function fn_isSupportedInputFormat

// Local Function
bool fn_isSupportedOutputFormat(const std::string& sExtension) 
{ // Begin fn_isSupportedOutputFormat
    if (sExtension.empty()) {
        return false;
    }
    
    std::string sLowerExtension = sExtension; // Local Function
    std::transform(sLowerExtension.begin(), sLowerExtension.end(), sLowerExtension.begin(), // In algorithm
                   [](unsigned char c) { return std::tolower(c); }); // In cctype
    
    // Add dot for comparison if not present
    if (!sLowerExtension.empty() && sLowerExtension[0] != '.') {
        sLowerExtension = "." + sLowerExtension;
    }
    
    for (const auto& sSupportedFormat : vsSUPPORTED_OUTPUT_FORMATS) 
    { // Begin for
        if (sLowerExtension == sSupportedFormat) 
        { // Begin if
            return true; // Format is supported
        } // End if(sLowerExtension == sSupportedFormat)
    } // End for(const auto& sSupportedFormat : vsSUPPORTED_OUTPUT_FORMATS)
    
    return false; // Format not supported
} // End Function fn_isSupportedOutputFormat

// Local Function
std::string fn_normalizeExtension(const std::string& sExtension) 
{ // Begin fn_normalizeExtension
    std::string sNormalized = sExtension; // Local Function
    
    // Ensure extension starts with a dot
    if (!sNormalized.empty() && sNormalized[0] != '.') 
    { // Begin if
        sNormalized = "." + sNormalized; // Add dot prefix
    } // End if(!sNormalized.empty() && sNormalized[0] != '.')
    
    // Convert to lowercase
    std::transform(sNormalized.begin(), sNormalized.end(), sNormalized.begin(), // In algorithm
                   [](unsigned char c) { return std::tolower(c); }); // In cctype
    
    return sNormalized; // Return normalized extension
} // End Function fn_normalizeExtension

// Local Function
std::string fn_getMimeTypeForExtension(const std::string& sExtension) 
{ // Begin fn_getMimeTypeForExtension
    std::string sNormalizedExtension = fn_normalizeExtension(sExtension); // Local Function
    
    for (const auto& oFormat : voFORMAT_MIME_TYPES) 
    { // Begin for
        if (oFormat.sExtension == sNormalizedExtension) 
        { // Begin if
            return oFormat.sMimeType; // Return corresponding MIME type
        } // End if(oFormat.sExtension == sNormalizedExtension)
    } // End for(const auto& oFormat : voFORMAT_MIME_TYPES)
    
    return "application/octet-stream"; // Default MIME type
} // End Function fn_getMimeTypeForExtension

// Local Function
std::string fn_getExtensionForMimeType(const std::string& sMimeType) 
{ // Begin fn_getExtensionForMimeType
    for (const auto& oFormat : voFORMAT_MIME_TYPES) 
    { // Begin for
        if (oFormat.sMimeType == sMimeType) 
        { // Begin if
            return oFormat.sExtension; // Return corresponding extension
        } // End if(oFormat.sMimeType == sMimeType)
    } // End for(const auto& oFormat : voFORMAT_MIME_TYPES)
    
    return ""; // Empty string if not found
} // End Function fn_getExtensionForMimeType

// Local Function
oConfig fn_getDefaultConfig() 
{ // Begin fn_getDefaultConfig
    oConfig oDefaultConfig; // Local Function
    
    oDefaultConfig.sInputPath = ""; // Local Function
    oDefaultConfig.sOutputPath = ""; // Local Function
    oDefaultConfig.sOutputFormat = fn_getDefaultOutputFormat(); // Local Function
    oDefaultConfig.iJpegQuality = iDEFAULT_JPEG_QUALITY; // Local Function
    oDefaultConfig.iPngCompression = iDEFAULT_PNG_COMPRESSION; // Local Function
    oDefaultConfig.iThreadCount = iDEFAULT_THREAD_COUNT; // Local Function
    oDefaultConfig.fScaleFactor = fDEFAULT_SCALE_FACTOR; // Local Function
    oDefaultConfig.bOverwrite = bDEFAULT_OVERWRITE; // Local Function
    oDefaultConfig.bVerbose = bDEFAULT_VERBOSE; // Local Function
    oDefaultConfig.bRecursive = bDEFAULT_RECURSIVE; // Local Function
    oDefaultConfig.bKeepMetadata = true; // Local Function
    oDefaultConfig.bStripColorProfile = false; // Local Function
    
    return oDefaultConfig; // Return default configuration
} // End Function fn_getDefaultConfig

// Local Function
std::string fn_getEmbeddedCodecPath() 
{ // Begin fn_getEmbeddedCodecPath
    #ifdef DEBIAN9_BUILD
        return sDEBIAN9_CODEC_PATH; // Return Debian 9 codec path
    #elif defined(DEBIAN12_BUILD)
        return sDEBIAN12_CODEC_PATH; // Return Debian 12 codec path
    #else
        return sEMBEDDED_CODEC_PATH; // Return generic codec path
    #endif
} // End Function fn_getEmbeddedCodecPath

// Local Function
void fn_printConfig(const oConfig& oCurrentConfig) 
{ // Begin fn_printConfig
    std::cout << "Current Configuration:" << std::endl; // In iostream
    std::cout << "  Input Path: " << oCurrentConfig.sInputPath << std::endl; // In iostream
    std::cout << "  Output Path: " << oCurrentConfig.sOutputPath << std::endl; // In iostream
    std::cout << "  Output Format: " << oCurrentConfig.sOutputFormat << std::endl; // In iostream
    std::cout << "  JPEG Quality: " << oCurrentConfig.iJpegQuality << std::endl; // In iostream
    std::cout << "  PNG Compression: " << oCurrentConfig.iPngCompression << std::endl; // In iostream
    std::cout << "  Thread Count: " << oCurrentConfig.iThreadCount << std::endl; // In iostream
    std::cout << "  Scale Factor: " << oCurrentConfig.fScaleFactor << std::endl; // In iostream
    std::cout << "  Overwrite: " << (oCurrentConfig.bOverwrite ? "true" : "false") << std::endl; // In iostream
    std::cout << "  Verbose: " << (oCurrentConfig.bVerbose ? "true" : "false") << std::endl; // In iostream
    std::cout << "  Recursive: " << (oCurrentConfig.bRecursive ? "true" : "false") << std::endl; // In iostream
    std::cout << "  Keep Metadata: " << (oCurrentConfig.bKeepMetadata ? "true" : "false") << std::endl; // In iostream
    std::cout << "  Strip Color Profile: " << (oCurrentConfig.bStripColorProfile ? "true" : "false") << std::endl; // In iostream
} // End Function fn_printConfig