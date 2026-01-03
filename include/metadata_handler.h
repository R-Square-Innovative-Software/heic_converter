#ifndef METADATA_HANDLER_H
#define METADATA_HANDLER_H

#include <vector>
#include <string>
#include <memory>
#include <ctime>

#ifdef HAVE_LIBHEIF
#include <libheif/heif.h>
#endif

class MetadataHandler {
public:
    MetadataHandler();
    ~MetadataHandler();
    
    // Extract metadata from HEIC/HEIF file
    std::vector<unsigned char> extractExifFromHeic(const std::string& filepath);
    
    // Extract metadata from HEIC/HEIF memory buffer
    std::vector<unsigned char> extractExifFromHeicData(const std::vector<unsigned char>& data);
    std::vector<unsigned char> extractXmpFromHeic(const std::string& filepath);
    
    // Write EXIF to JPEG file
    bool writeExifToJpeg(const std::string& jpegFile, const std::vector<unsigned char>& exifData);
    
    // Copy file timestamps
    bool copyTimestamps(const std::string& sourceFile, const std::string& destFile);
    
    // Get file creation time
    time_t getFileCreationTime(const std::string& filepath);
    
    // Get file modification time
    time_t getFileModificationTime(const std::string& filepath);
    
    // Set file timestamps
    bool setFileTimestamps(const std::string& filepath, time_t creation, time_t modification);
    
private:
    #ifdef HAVE_LIBHEIF
    struct heif_context* context;
    #endif
};

#endif // METADATA_HANDLER_H