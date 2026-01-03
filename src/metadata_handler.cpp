#include "metadata_handler.h"
#include "logger.h"
#include <fstream>
#include <vector>
#include <cstring>
#include <sys/stat.h>
#include <utime.h>
#include <iostream>

#ifdef HAVE_LIBHEIF
#include <libheif/heif.h>
#endif

MetadataHandler::MetadataHandler() {
    #ifdef HAVE_LIBHEIF
    context = nullptr;
    #endif
}

MetadataHandler::~MetadataHandler() {
    #ifdef HAVE_LIBHEIF
    if (context) {
        heif_context_free(context);
        context = nullptr;
    }
    #endif
}

std::vector<unsigned char> MetadataHandler::extractExifFromHeic(const std::string& filepath) {
    std::vector<unsigned char> exifData;
    
    #ifdef HAVE_LIBHEIF
    struct heif_error err;
    
    // Create context
    struct heif_context* ctx = heif_context_alloc();
    if (!ctx) {
        fn_logError("Failed to allocate HEIF context");
        return exifData;
    }
    
    // Read file
    err = heif_context_read_from_file(ctx, filepath.c_str(), nullptr);
    if (err.code != heif_error_Ok) {
        fn_logError(std::string("Failed to read HEIF file: ") + err.message);
        heif_context_free(ctx);
        return exifData;
    }
    
    // Get primary image handle
    struct heif_image_handle* handle = nullptr;
    err = heif_context_get_primary_image_handle(ctx, &handle);
    if (err.code != heif_error_Ok) {
        fn_logError(std::string("Failed to get image handle: ") + err.message);
        heif_context_free(ctx);
        return exifData;
    }
    
    // Get EXIF metadata
    heif_item_id exif_id;
    int count = heif_image_handle_get_list_of_metadata_block_IDs(handle, "Exif", &exif_id, 1);
    
    if (count > 0) {
        size_t exif_size = heif_image_handle_get_metadata_size(handle, exif_id);
        exifData.resize(exif_size);
        
        err = heif_image_handle_get_metadata(handle, exif_id, exifData.data());
        if (err.code != heif_error_Ok) {
            fn_logError("Failed to read EXIF data");
            exifData.clear();
        } else {
            fn_logInfo("Raw EXIF size from libheif: " + std::to_string(exif_size));
            
            // Check the structure
            if (exif_size >= 10) {
                // HEIF stores EXIF with a 4-byte length prefix
                // The format is: [4-byte length][6-byte "Exif\0\0"][TIFF data]
                // We need to strip the 4-byte length for JPEG APP1 segment
                
                // Verify it has the expected structure
                if (exifData[4] == 'E' && exifData[5] == 'x' && 
                    exifData[6] == 'i' && exifData[7] == 'f' &&
                    exifData[8] == 0 && exifData[9] == 0) {
                    
                    // Extract the length (big-endian)
                    uint32_t length_prefix = (exifData[0] << 24) | 
                                            (exifData[1] << 16) | 
                                            (exifData[2] << 8) | 
                                            exifData[3];
                    
                    fn_logInfo("HEIF EXIF length prefix: " + std::to_string(length_prefix));
                    
                    // Remove the 4-byte length prefix
                    std::vector<unsigned char> cleanExifData;
                    cleanExifData.reserve(exif_size - 4);
                    
                    // Start from byte 4 (skip the length prefix)
                    cleanExifData.insert(cleanExifData.end(), 
                                        exifData.begin() + 4, 
                                        exifData.end());
                    
                    exifData = cleanExifData;
                    fn_logInfo("Removed 4-byte length prefix, new size: " + std::to_string(exifData.size()));
                    
                    // Verify the TIFF header
                    if (exifData.size() >= 8) {
                        if (exifData[6] == 'I' && exifData[7] == 'I') {
                            fn_logInfo("TIFF header: II (Intel, little-endian)");
                        } else if (exifData[6] == 'M' && exifData[7] == 'M') {
                            fn_logInfo("TIFF header: MM (Motorola, big-endian)");
                        } else {
                            fn_logWarning("Invalid TIFF header after cleanup");
                        }
                    }
                } else {
                    fn_logWarning("EXIF data doesn't have expected structure");
                }
            }
        }
    } else {
        fn_logInfo("No EXIF metadata found in HEIC file");
    }
    
    // Cleanup
    heif_image_handle_release(handle);
    heif_context_free(ctx);
    
    #else
    fn_logWarning("libheif not available for metadata extraction");
    #endif
    
    fn_logInfo("Final EXIF data size: " + std::to_string(exifData.size()) + " bytes");
    return exifData;
}

std::vector<unsigned char> MetadataHandler::extractXmpFromHeic(const std::string& filepath) {
    std::vector<unsigned char> xmpData;
    
    #ifdef HAVE_LIBHEIF
    struct heif_error err;
    struct heif_context* ctx = heif_context_alloc();
    if (!ctx) return xmpData;
    
    err = heif_context_read_from_file(ctx, filepath.c_str(), nullptr);
    if (err.code != heif_error_Ok) {
        heif_context_free(ctx);
        return xmpData;
    }
    
    struct heif_image_handle* handle = nullptr;
    err = heif_context_get_primary_image_handle(ctx, &handle);
    if (err.code != heif_error_Ok) {
        heif_context_free(ctx);
        return xmpData;
    }
    
    // Get XMP metadata
    heif_item_id xmp_id;
    int count = heif_image_handle_get_list_of_metadata_block_IDs(handle, "mime", &xmp_id, 1);
    
    if (count > 0) {
        const char* mime_type = heif_image_handle_get_metadata_type(handle, xmp_id);
        if (mime_type && strstr(mime_type, "application/rdf+xml")) {
            size_t xmp_size = heif_image_handle_get_metadata_size(handle, xmp_id);
            xmpData.resize(xmp_size);
            err = heif_image_handle_get_metadata(handle, xmp_id, xmpData.data());
            if (err.code != heif_error_Ok) {
                xmpData.clear();
            }
        }
    }
    
    heif_image_handle_release(handle);
    heif_context_free(ctx);
    #endif
    
    return xmpData;
}

bool MetadataHandler::writeExifToJpeg(const std::string& jpegFile, const std::vector<unsigned char>& exifData) {
    if (exifData.empty()) {
        return true;
    }
    
    // Use exiftool to do it correctly (if available)
    // This is the most reliable approach
    std::string tempFile = jpegFile + ".tmp";
    
    // First copy the JPEG without EXIF
    std::ifstream in(jpegFile, std::ios::binary);
    std::ofstream out(tempFile, std::ios::binary);
    out << in.rdbuf();
    in.close();
    out.close();
    
    // Write EXIF to a temporary file
    std::string exifFile = jpegFile + ".exif";
    std::ofstream exifOut(exifFile, std::ios::binary);
    exifOut.write(reinterpret_cast<const char*>(exifData.data()), exifData.size());
    exifOut.close();
    
    // Try using exiftool if available
    std::string command = "which exiftool > /dev/null 2>&1";
    if (system(command.c_str()) == 0) {
        command = "exiftool -overwrite_original -tagsFromFile \"\" -exif:all=\"\" \"" + tempFile + "\" > /dev/null 2>&1";
        
        command = "exiftool -overwrite_original -exif=\"<" + exifFile + "\" \"" + tempFile + "\" > /dev/null 2>&1";
        if (system(command.c_str()) == 0) {
            // Replace original with temp file
            std::rename(tempFile.c_str(), jpegFile.c_str());
            std::remove(exifFile.c_str());
            fn_logInfo("Used exiftool to write EXIF metadata");
            return true;
        }
    }
    
    // Fall back to our implementation
    std::remove(tempFile.c_str());
    std::remove(exifFile.c_str());
    
    // Read the entire JPEG file
    std::ifstream inputFile(jpegFile, std::ios::binary);
    if (!inputFile.is_open()) {
        fn_logError("Cannot open JPEG file for reading: " + jpegFile);
        return false;
    }
    
    inputFile.seekg(0, std::ios::end);
    size_t fileSize = inputFile.tellg();
    inputFile.seekg(0, std::ios::beg);
    
    std::vector<unsigned char> fileData(fileSize);
    inputFile.read(reinterpret_cast<char*>(fileData.data()), fileSize);
    inputFile.close();
    
    // Check if it's a valid JPEG
    if (fileData.size() < 2 || fileData[0] != 0xFF || fileData[1] != 0xD8) {
        fn_logError("Not a valid JPEG file: " + jpegFile);
        return false;
    }
    
    // Create EXIF data with proper header if needed
    std::vector<unsigned char> exifToWrite;
    
    // Check if EXIF data already has header
    bool hasHeader = (exifData.size() >= 6 && 
                     exifData[0] == 'E' && exifData[1] == 'x' && 
                     exifData[2] == 'i' && exifData[3] == 'f' &&
                     exifData[4] == 0 && exifData[5] == 0);
    
    if (hasHeader) {
        exifToWrite = exifData;
        fn_logInfo("EXIF data already has proper header");
    } else {
        fn_logInfo("Adding EXIF header to data");
        // Add EXIF header: "Exif\0\0"
        exifToWrite.push_back('E');
        exifToWrite.push_back('x');
        exifToWrite.push_back('i');
        exifToWrite.push_back('f');
        exifToWrite.push_back(0);
        exifToWrite.push_back(0);
        exifToWrite.insert(exifToWrite.end(), exifData.begin(), exifData.end());
    }
    
    // Build new file data with EXIF
    std::vector<unsigned char> newFileData;
    newFileData.reserve(fileSize + exifToWrite.size() + 10);
    
    // Write SOI marker (Start of Image)
    newFileData.push_back(0xFF);
    newFileData.push_back(0xD8);
    
    // Write APP1 marker for EXIF
    newFileData.push_back(0xFF);
    newFileData.push_back(0xE1); // APP1 marker
    
    // Write APP1 segment length (big-endian)
    uint16_t segmentLength = static_cast<uint16_t>(exifToWrite.size() + 2);
    newFileData.push_back(static_cast<unsigned char>((segmentLength >> 8) & 0xFF));
    newFileData.push_back(static_cast<unsigned char>(segmentLength & 0xFF));
    
    // Write EXIF data
    newFileData.insert(newFileData.end(), exifToWrite.begin(), exifToWrite.end());
    
    // Skip existing markers in original file
    size_t pos = 2; // Start after SOI
    
    while (pos < fileData.size()) {
        if (fileData[pos] != 0xFF) {
            break;
        }
        
        unsigned char marker = fileData[pos + 1];
        
        // Skip APP0 (JFIF) and APP1 (EXIF) markers
        if (marker == 0xE0 || marker == 0xE1) {
            if (pos + 3 >= fileData.size()) break;
            uint16_t markerLength = (fileData[pos + 2] << 8) | fileData[pos + 3];
            pos += markerLength + 2;
        } else {
            break;
        }
    }
    
    // Copy the rest of the image data
    newFileData.insert(newFileData.end(), fileData.begin() + pos, fileData.end());
    
    // Write the new file
    std::ofstream outputFile(jpegFile, std::ios::binary);
    if (!outputFile.is_open()) {
        fn_logError("Cannot open JPEG file for writing: " + jpegFile);
        return false;
    }
    
    outputFile.write(reinterpret_cast<const char*>(newFileData.data()), newFileData.size());
    outputFile.close();
    
    fn_logInfo("Successfully wrote EXIF data to JPEG: " + jpegFile);
    return true;
}

bool MetadataHandler::copyTimestamps(const std::string& sourceFile, const std::string& destFile) {
    struct stat sourceStat;
    if (stat(sourceFile.c_str(), &sourceStat) != 0) {
        fn_logError("Failed to get source file stats: " + sourceFile);
        return false;
    }
    
    struct utimbuf newTimes;
    newTimes.actime = sourceStat.st_atime;
    newTimes.modtime = sourceStat.st_mtime;
    
    if (utime(destFile.c_str(), &newTimes) != 0) {
        fn_logWarning("Failed to set timestamps for: " + destFile);
        return false;
    }
    
    fn_logInfo("Copied timestamps from " + sourceFile + " to " + destFile);
    return true;
}

time_t MetadataHandler::getFileCreationTime(const std::string& filepath) {
    struct stat fileStat;
    if (stat(filepath.c_str(), &fileStat) == 0) {
        #ifdef __APPLE__
        return fileStat.st_birthtime;
        #elif defined(__linux__)
        // Try to get birth time from statx if available
        return fileStat.st_mtime; // Fallback to modification time
        #else
        return fileStat.st_ctime;
        #endif
    }
    return time(nullptr);
}

time_t MetadataHandler::getFileModificationTime(const std::string& filepath) {
    struct stat fileStat;
    if (stat(filepath.c_str(), &fileStat) == 0) {
        return fileStat.st_mtime;
    }
    return time(nullptr);
}

bool MetadataHandler::setFileTimestamps(const std::string& filepath, time_t creation, time_t modification) {
    struct utimbuf newTimes;
    newTimes.actime = creation;  // Use creation time for access time
    newTimes.modtime = modification;
    
    if (utime(filepath.c_str(), &newTimes) == 0) {
        return true;
    }
    return false;
}