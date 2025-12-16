// format_encoder.cpp
// R Square Innovation Software - HEIC Converter v1.0
// Implementation of format encoding using system libraries

#include "format_encoder.h"
#include "logger.h"
#include <vector>
#include <string>
#include <cstring>
#include <fstream>

// External libraries (system installed)
#ifdef HAVE_PNG
#include <png.h>
#endif

#ifdef HAVE_JPEG
#include <jpeglib.h>
#include <jerror.h>
#endif

#ifdef HAVE_WEBP
#include <webp/encode.h>
#endif

#ifdef HAVE_TIFF
#include <tiffio.h>
#endif

// Constructor
FormatEncoder::FormatEncoder() {
    // Initialize support flags
    bPNGSupported = fn_checkPNGSupport();
    bJPEGSupported = fn_checkJPEGSupport();
    bWebPSupported = fn_checkWebPSupport();
    bBMPSupported = fn_checkBMPSupport();
    bTIFFSupported = fn_checkTIFFSupport();
}
// End Constructor

// Destructor
FormatEncoder::~FormatEncoder() {
    // Cleanup if needed
}
// End Destructor

// Main encoding function
bool FormatEncoder::fn_encodeImage(
    const sImageData& oImageData,
    const std::string& sOutputPath,
    const sEncodeOptions& oOptions
) {
    // Local Function
    
    // Validate input
    if (!oImageData.pData) {
        fn_logError("Invalid image data pointer");
        return false;
    }
    
    if (oImageData.iWidth <= 0 || oImageData.iHeight <= 0) {
        fn_logError("Invalid image dimensions");
        return false;
    }
    
    if (oImageData.iChannels < 1 || oImageData.iChannels > 4) {
        fn_logError("Invalid number of channels");
        return false;
    }
    
    // Validate format
    if (!fn_validateFormat(oOptions.sFormat)) {
        fn_logError("Unsupported format: " + oOptions.sFormat);
        return false;
    }
    
    // Route to appropriate encoder
    std::string sFormatLower = oOptions.sFormat;
    for (char& c : sFormatLower) {
        c = std::tolower(c);
    }
    
    bool bSuccess = false;
    
    if (sFormatLower == "png") {
        bSuccess = fn_encodePNG(oImageData, sOutputPath, oOptions);
    }
    else if (sFormatLower == "jpg" || sFormatLower == "jpeg") {
        bSuccess = fn_encodeJPEG(oImageData, sOutputPath, oOptions);
    }
    else if (sFormatLower == "webp") {
        bSuccess = fn_encodeWebP(oImageData, sOutputPath, oOptions);
    }
    else if (sFormatLower == "bmp") {
        bSuccess = fn_encodeBMP(oImageData, sOutputPath, oOptions);
    }
    else if (sFormatLower == "tiff" || sFormatLower == "tif") {
        bSuccess = fn_encodeTIFF(oImageData, sOutputPath, oOptions);
    }
    else {
        fn_logError("Unknown format: " + oOptions.sFormat);
        return false;
    }
    
    if (bSuccess) {
        fn_logInfo("Successfully encoded image to: " + sOutputPath);
    }
    
    return bSuccess;
}
// End Function fn_encodeImage

// Get supported formats
std::vector<std::string> FormatEncoder::fn_getSupportedFormats() {
    // Local Function
    
    std::vector<std::string> vsFormats;
    
    if (bPNGSupported) {
        vsFormats.push_back("png");
    }
    
    if (bJPEGSupported) {
        vsFormats.push_back("jpg");
        vsFormats.push_back("jpeg");
    }
    
    if (bWebPSupported) {
        vsFormats.push_back("webp");
    }
    
    if (bBMPSupported) {
        vsFormats.push_back("bmp");
    }
    
    if (bTIFFSupported) {
        vsFormats.push_back("tiff");
        vsFormats.push_back("tif");
    }
    
    return vsFormats;
}
// End Function fn_getSupportedFormats

// Validate output format
bool FormatEncoder::fn_validateFormat(const std::string& sFormat) {
    // Local Function
    
    std::string sFormatLower = sFormat;
    for (char& c : sFormatLower) {
        c = std::tolower(c);
    }
    
    if (sFormatLower == "png" && bPNGSupported) {
        return true;
    }
    else if ((sFormatLower == "jpg" || sFormatLower == "jpeg") && bJPEGSupported) {
        return true;
    }
    else if (sFormatLower == "webp" && bWebPSupported) {
        return true;
    }
    else if (sFormatLower == "bmp" && bBMPSupported) {
        return true;
    }
    else if ((sFormatLower == "tiff" || sFormatLower == "tif") && bTIFFSupported) {
        return true;
    }
    
    return false;
}
// End Function fn_validateFormat

// PNG encoding function
bool FormatEncoder::fn_encodePNG(
    const sImageData& oImageData,
    const std::string& sOutputPath,
    const sEncodeOptions& oOptions
) {
    // Local Function
    
    #ifdef HAVE_PNG
    FILE* fp = fopen(sOutputPath.c_str(), "wb");
    if (!fp) {
        fn_logError("Cannot open file for writing: " + sOutputPath);
        return false;
    }
    
    png_structp pPNG = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!pPNG) {
        fclose(fp);
        fn_logError("Failed to create PNG write structure");
        return false;
    }
    
    png_infop pInfo = png_create_info_struct(pPNG);
    if (!pInfo) {
        png_destroy_write_struct(&pPNG, nullptr);
        fclose(fp);
        fn_logError("Failed to create PNG info structure");
        return false;
    }
    
    if (setjmp(png_jmpbuf(pPNG))) {
        png_destroy_write_struct(&pPNG, &pInfo);
        fclose(fp);
        fn_logError("Error during PNG creation");
        return false;
    }
    
    png_init_io(pPNG, fp);
    
    // Set color type based on channels
    int iColorType;
    if (oImageData.iChannels == 1) {
        iColorType = PNG_COLOR_TYPE_GRAY;
    }
    else if (oImageData.iChannels == 2) {
        iColorType = PNG_COLOR_TYPE_GRAY_ALPHA;
    }
    else if (oImageData.iChannels == 3) {
        iColorType = PNG_COLOR_TYPE_RGB;
    }
    else if (oImageData.iChannels == 4) {
        iColorType = PNG_COLOR_TYPE_RGB_ALPHA;
    }
    else {
        png_destroy_write_struct(&pPNG, &pInfo);
        fclose(fp);
        fn_logError("Unsupported channel count for PNG");
        return false;
    }
    
    png_set_IHDR(
        pPNG,
        pInfo,
        oImageData.iWidth,
        oImageData.iHeight,
        oImageData.iBitDepth,
        iColorType,
        PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT
    );
    
    // Set compression level
    if (oOptions.iCompressionLevel >= 0 && oOptions.iCompressionLevel <= 9) {
        png_set_compression_level(pPNG, oOptions.iCompressionLevel);
    }
    
    // Set interlace if requested
    if (oOptions.bInterlace) {
        png_set_IHDR(
            pPNG,
            pInfo,
            oImageData.iWidth,
            oImageData.iHeight,
            oImageData.iBitDepth,
            iColorType,
            PNG_INTERLACE_ADAM7,
            PNG_COMPRESSION_TYPE_DEFAULT,
            PNG_FILTER_TYPE_DEFAULT
        );
    }
    
    png_write_info(pPNG, pInfo);
    
    // Write image data
    png_bytep* ppRowPointers = new png_bytep[oImageData.iHeight];
    int iRowBytes = png_get_rowbytes(pPNG, pInfo);
    
    for (int i = 0; i < oImageData.iHeight; i++) {
        ppRowPointers[i] = oImageData.pData + (i * iRowBytes);
    }
    
    png_write_image(pPNG, ppRowPointers);
    png_write_end(pPNG, nullptr);
    
    // Cleanup
    delete[] ppRowPointers;
    png_destroy_write_struct(&pPNG, &pInfo);
    fclose(fp);
    
    return true;
    #else
    fn_logError("PNG support not compiled in");
    return false;
    #endif
}
// End Function fn_encodePNG

// JPEG encoding function
bool FormatEncoder::fn_encodeJPEG(
    const sImageData& oImageData,
    const std::string& sOutputPath,
    const sEncodeOptions& oOptions
) {
    // Local Function
    
    #ifdef HAVE_JPEG
    FILE* fp = fopen(sOutputPath.c_str(), "wb");
    if (!fp) {
        fn_logError("Cannot open file for writing: " + sOutputPath);
        return false;
    }
    
    struct jpeg_compress_struct sCInfo;
    struct jpeg_error_mgr sJErr;
    
    sCInfo.err = jpeg_std_error(&sJErr);
    jpeg_create_compress(&sCInfo);
    jpeg_stdio_dest(&sCInfo, fp);
    
    sCInfo.image_width = oImageData.iWidth;
    sCInfo.image_height = oImageData.iHeight;
    
    if (oImageData.iChannels == 1) {
        sCInfo.input_components = 1;
        sCInfo.in_color_space = JCS_GRAYSCALE;
    }
    else if (oImageData.iChannels == 3) {
        sCInfo.input_components = 3;
        sCInfo.in_color_space = JCS_RGB;
    }
    else {
        jpeg_destroy_compress(&sCInfo);
        fclose(fp);
        fn_logError("JPEG only supports 1 (grayscale) or 3 (RGB) channels");
        return false;
    }
    
    jpeg_set_defaults(&sCInfo);
    
    // Set quality
    int iQuality = oOptions.iQuality;
    if (iQuality < 1) iQuality = 1;
    if (iQuality > 100) iQuality = 100;
    jpeg_set_quality(&sCInfo, iQuality, TRUE);
    
    // Set progressive if requested
    if (oOptions.bProgressive) {
        jpeg_simple_progression(&sCInfo);
    }
    
    jpeg_start_compress(&sCInfo, TRUE);
    
    // Write scanlines
    JSAMPROW pRowPointer[1];
    int iRowStride = oImageData.iWidth * oImageData.iChannels;
    
    while (sCInfo.next_scanline < sCInfo.image_height) {
        pRowPointer[0] = &oImageData.pData[sCInfo.next_scanline * iRowStride];
        jpeg_write_scanlines(&sCInfo, pRowPointer, 1);
    }
    
    jpeg_finish_compress(&sCInfo);
    jpeg_destroy_compress(&sCInfo);
    fclose(fp);
    
    return true;
    #else
    fn_logError("JPEG support not compiled in");
    return false;
    #endif
}
// End Function fn_encodeJPEG

// WebP encoding function
bool FormatEncoder::fn_encodeWebP(
    const sImageData& oImageData,
    const std::string& sOutputPath,
    const sEncodeOptions& oOptions
) {
    // Local Function
    
    #ifdef HAVE_WEBP
    uint8_t* pOutput = nullptr;
    size_t iSize = 0;
    
    if (oOptions.bLossless) {
        // Lossless encoding
        iSize = WebPEncodeLosslessRGBA(
            oImageData.pData,
            oImageData.iWidth,
            oImageData.iHeight,
            oImageData.iWidth * oImageData.iChannels,
            &pOutput
        );
    }
    else {
        // Lossy encoding
        float fQuality = static_cast<float>(oOptions.iQuality);
        if (fQuality < 0.0f) fQuality = 0.0f;
        if (fQuality > 100.0f) fQuality = 100.0f;
        
        if (oImageData.iChannels == 4) {
            iSize = WebPEncodeRGBA(
                oImageData.pData,
                oImageData.iWidth,
                oImageData.iHeight,
                oImageData.iWidth * oImageData.iChannels,
                fQuality,
                &pOutput
            );
        }
        else if (oImageData.iChannels == 3) {
            iSize = WebPEncodeRGB(
                oImageData.pData,
                oImageData.iWidth,
                oImageData.iHeight,
                oImageData.iWidth * oImageData.iChannels,
                fQuality,
                &pOutput
            );
        }
        else {
            fn_logError("WebP only supports RGB or RGBA");
            return false;
        }
    }
    
    if (iSize == 0 || !pOutput) {
        fn_logError("WebP encoding failed");
        return false;
    }
    
    // Write to file
    std::ofstream oFile(sOutputPath, std::ios::binary);
    if (!oFile) {
        WebPFree(pOutput);
        fn_logError("Cannot open file for writing: " + sOutputPath);
        return false;
    }
    
    oFile.write(reinterpret_cast<char*>(pOutput), iSize);
    oFile.close();
    WebPFree(pOutput);
    
    return true;
    #else
    fn_logError("WebP support not compiled in");
    return false;
    #endif
}
// End Function fn_encodeWebP

// BMP encoding function
bool FormatEncoder::fn_encodeBMP(
    const sImageData& oImageData,
    const std::string& sOutputPath,
    const sEncodeOptions& oOptions
) {
    // Local Function
    
    #pragma pack(push, 1)
    struct sBMPFileHeader {
        uint16_t bfType = 0x4D42; // "BM"
        uint32_t bfSize;
        uint16_t bfReserved1 = 0;
        uint16_t bfReserved2 = 0;
        uint32_t bfOffBits;
    };
    
    struct sBMPInfoHeader {
        uint32_t biSize = 40;
        int32_t biWidth;
        int32_t biHeight;
        uint16_t biPlanes = 1;
        uint16_t biBitCount;
        uint32_t biCompression = 0; // BI_RGB
        uint32_t biSizeImage;
        int32_t biXPelsPerMeter = 0;
        int32_t biYPelsPerMeter = 0;
        uint32_t biClrUsed = 0;
        uint32_t biClrImportant = 0;
    };
    #pragma pack(pop)
    
    // Only support RGB and RGBA for BMP
    if (oImageData.iChannels != 3 && oImageData.iChannels != 4) {
        fn_logError("BMP only supports RGB or RGBA");
        return false;
    }
    
    // BMP requires BGR order
    int iWidth = oImageData.iWidth;
    int iHeight = oImageData.iHeight;
    int iChannels = oImageData.iChannels;
    
    // Calculate padding
    int iRowPadding = (4 - (iWidth * iChannels) % 4) % 4;
    int iRowSize = iWidth * iChannels + iRowPadding;
    
    sBMPFileHeader oFileHeader;
    sBMPInfoHeader oInfoHeader;
    
    oInfoHeader.biWidth = iWidth;
    oInfoHeader.biHeight = iHeight;
    oInfoHeader.biBitCount = iChannels * 8;
    oInfoHeader.biSizeImage = iRowSize * iHeight;
    
    oFileHeader.bfOffBits = sizeof(sBMPFileHeader) + sizeof(sBMPInfoHeader);
    oFileHeader.bfSize = oFileHeader.bfOffBits + oInfoHeader.biSizeImage;
    
    std::ofstream oFile(sOutputPath, std::ios::binary);
    if (!oFile) {
        fn_logError("Cannot open file for writing: " + sOutputPath);
        return false;
    }
    
    // Write headers
    oFile.write(reinterpret_cast<char*>(&oFileHeader), sizeof(oFileHeader));
    oFile.write(reinterpret_cast<char*>(&oInfoHeader), sizeof(oInfoHeader));
    
    // Write image data (BMP is bottom-up, BGR order)
    for (int y = iHeight - 1; y >= 0; y--) {
        const unsigned char* pRow = oImageData.pData + y * iWidth * iChannels;
        
        for (int x = 0; x < iWidth; x++) {
            // Convert RGB to BGR
            if (iChannels >= 3) {
                oFile.put(pRow[x * iChannels + 2]); // B
                oFile.put(pRow[x * iChannels + 1]); // G
                oFile.put(pRow[x * iChannels + 0]); // R
                
                if (iChannels == 4) {
                    oFile.put(pRow[x * iChannels + 3]); // A
                }
            }
        }
        
        // Write padding
        for (int p = 0; p < iRowPadding; p++) {
            oFile.put(0);
        }
    }
    
    oFile.close();
    return true;
}
// End Function fn_encodeBMP

// TIFF encoding function
bool FormatEncoder::fn_encodeTIFF(
    const sImageData& oImageData,
    const std::string& sOutputPath,
    const sEncodeOptions& oOptions
) {
    // Local Function
    
    #ifdef HAVE_TIFF
    TIFF* pTiff = TIFFOpen(sOutputPath.c_str(), "w");
    if (!pTiff) {
        fn_logError("Cannot open TIFF file for writing: " + sOutputPath);
        return false;
    }
    
    // Set basic tags
    TIFFSetField(pTiff, TIFFTAG_IMAGEWIDTH, oImageData.iWidth);
    TIFFSetField(pTiff, TIFFTAG_IMAGELENGTH, oImageData.iHeight);
    TIFFSetField(pTiff, TIFFTAG_BITSPERSAMPLE, oImageData.iBitDepth);
    TIFFSetField(pTiff, TIFFTAG_SAMPLESPERPIXEL, oImageData.iChannels);
    TIFFSetField(pTiff, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
    TIFFSetField(pTiff, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    TIFFSetField(pTiff, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
    TIFFSetField(pTiff, TIFFTAG_COMPRESSION, COMPRESSION_LZW);
    
    // Set compression if specified
    if (oOptions.iCompressionLevel > 0) {
        // Adjust compression based on level
        if (oOptions.iCompressionLevel <= 3) {
            TIFFSetField(pTiff, TIFFTAG_COMPRESSION, COMPRESSION_LZW);
        }
        else if (oOptions.iCompressionLevel <= 6) {
            TIFFSetField(pTiff, TIFFTAG_COMPRESSION, COMPRESSION_DEFLATE);
        }
        else {
            TIFFSetField(pTiff, TIFFTAG_COMPRESSION, COMPRESSION_JPEG);
        }
    }
    
    // Set photometric based on channels
    if (oImageData.iChannels == 1) {
        TIFFSetField(pTiff, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
    }
    else if (oImageData.iChannels == 3 || oImageData.iChannels == 4) {
        TIFFSetField(pTiff, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
    }
    
    // Set extra samples for alpha
    if (oImageData.iChannels == 4) {
        uint16_t vExtraSamples = EXTRASAMPLE_ASSOCALPHA;
        TIFFSetField(pTiff, TIFFTAG_EXTRASAMPLES, 1, &vExtraSamples);
    }
    
    // Write image data
    int iRowSize = oImageData.iWidth * oImageData.iChannels * (oImageData.iBitDepth / 8);
    for (int iRow = 0; iRow < oImageData.iHeight; iRow++) {
        if (TIFFWriteScanline(pTiff, 
            oImageData.pData + (iRow * iRowSize), 
            iRow, 0) < 0) {
            TIFFClose(pTiff);
            fn_logError("Failed to write TIFF scanline");
            return false;
        }
    }
    
    TIFFClose(pTiff);
    return true;
    #else
    fn_logError("TIFF support not compiled in");
    return false;
    #endif
}
// End Function fn_encodeTIFF

// Check for PNG support
bool FormatEncoder::fn_checkPNGSupport() {
    // Local Function
    #ifdef HAVE_PNG
    return true;
    #else
    fn_logWarning("PNG support not available at compile time");
    return false;
    #endif
}
// End Function fn_checkPNGSupport

// Check for JPEG support
bool FormatEncoder::fn_checkJPEGSupport() {
    // Local Function
    #ifdef HAVE_JPEG
    return true;
    #else
    fn_logWarning("JPEG support not available at compile time");
    return false;
    #endif
}
// End Function fn_checkJPEGSupport

// Check for WebP support
bool FormatEncoder::fn_checkWebPSupport() {
    // Local Function
    #ifdef HAVE_WEBP
    return true;
    #else
    fn_logWarning("WebP support not available at compile time");
    return false;
    #endif
}
// End Function fn_checkWebPSupport

// Check for BMP support
bool FormatEncoder::fn_checkBMPSupport() {
    // Local Function
    // BMP is always supported (we implement it ourselves)
    return true;
}
// End Function fn_checkBMPSupport

// Check for TIFF support
bool FormatEncoder::fn_checkTIFFSupport() {
    // Local Function
    #ifdef HAVE_TIFF
    return true;
    #else
    fn_logWarning("TIFF support not available at compile time");
    return false;
    #endif
}
// End Function fn_checkTIFFSupport
// End format_encoder.cpp