#ifndef HEIC_DECODER_H
#define HEIC_DECODER_H

#include <string>
#include <vector>
#include <memory>

// Object to store decoded image data
struct oDecodedImage
{
    std::vector<unsigned char> vData; // Raw pixel data
    int iWidth;                       // Image width in pixels
    int iHeight;                      // Image height in pixels
    int iChannels;                    // Number of color channels (3 for RGB, 4 for RGBA)
    std::string sColorSpace;          // Color space information
    bool bHasAlpha;                   // Whether image has alpha channel
    std::string sError;               // Error message if decoding failed
}; // End struct oDecodedImage

// Object to store HEIC/HEIF image information
struct oHeicInfo
{
    std::string sFormat;              // Image format (HEIC, HEIF, etc.)
    int iWidth;                       // Image width in pixels
    int iHeight;                      // Image height in pixels
    int iBitDepth;                    // Bit depth
    std::string sColorSpace;          // Color space
    bool bHasAlpha;                   // Whether image has alpha channel
    int iOrientation;                 // EXIF orientation
    std::vector<std::string> vsMetadata; // Image metadata
}; // End struct oHeicInfo

class HeicDecoder
{
private:
    // Private variables
    std::string sLastError;                      // Last error message
    bool bInitialized;                           // Whether decoder is initialized
    std::string sEmbeddedCodecPath;              // Path to embedded codec data
    void* pDecoderContext;                       // Pointer to decoder context (opaque)
    std::vector<std::string> vsSupportedFormats; // List of supported formats
    
    // Private functions
    bool fn_initializeEmbeddedCodecs();          // Local Function
    bool fn_createDecoderContext();              // Local Function
    void fn_cleanupDecoderContext();             // Local Function
    
public:
    // Constructor and destructor
    HeicDecoder();                               // Local Function
    ~HeicDecoder();                              // Local Function
    
    // Main decoding functions
    oDecodedImage fn_decodeFile(const std::string& sFilePath);               // Local Function
    oDecodedImage fn_decodeMemory(const std::vector<unsigned char>& vData);  // Local Function
    
    // Information functions
    oHeicInfo fn_getImageInfo(const std::string& sFilePath);                // Local Function
    oHeicInfo fn_getImageInfoFromMemory(const std::vector<unsigned char>& vData); // Local Function
    
    // Utility functions
    bool fn_isFormatSupported(const std::string& sFormat);                  // Local Function
    std::vector<std::string> fn_getSupportedFormats();                      // Local Function
    std::string fn_getLastError();                                          // Local Function
    bool fn_isInitialized();                                                // Local Function
    
    // Configuration functions
    void fn_setEmbeddedCodecPath(const std::string& sPath);                // Local Function
    std::string fn_getEmbeddedCodecPath();                                 // Local Function
    
    // Batch functions
    std::vector<oDecodedImage> fn_decodeMultipleFiles(const std::vector<std::string>& vsFilePaths); // Local Function
    bool fn_saveDecodedImage(const oDecodedImage& oImage, const std::string& sOutputPath);         // Local Function
}; // End class HeicDecoder

#endif // HEIC_DECODER_H