// batch_processor.h - Batch processing for HEIC/HEIF converter
// Author: R Square Innovation Software
// Version: v1.0

#ifndef BATCH_PROCESSOR_H
#define BATCH_PROCESSOR_H

#include <vector>
#include <string>
#include "config.h"

class Converter; // Forward declaration

class BatchProcessor
{
public:
    // Constructor
    BatchProcessor();
    
    // Destructor
    ~BatchProcessor();
    
    // Process batch conversion - UPDATED to match implementation
    bool fn_processBatch(
        const std::vector<std::string>& vsInputFiles,
        const std::string& sOutputFormat,
        const std::string& sOutputDirectory,
        int iQuality,
        bool bPreserveMetadata,
        bool bVerbose
    );
    
    // Process directory conversion - UPDATED to match implementation
    bool fn_processDirectory(
        const std::string& sInputDirectory,
        const std::string& sOutputFormat,
        const std::string& sOutputDirectory,
        bool bRecursive,
        int iQuality,
        bool bPreserveMetadata,
        bool bVerbose
    );
    
    // Get processed file count
    int fn_getProcessedCount() const;
    
    // Get failed file count
    int fn_getFailedCount() const;
    
    // Get list of failed files
    std::vector<std::string> fn_getFailedFiles() const;
    
    // Clear statistics
    void fn_clearStatistics();
    
    // Batch size management - ADD THESE
    void fn_setBatchSize(int iBatchSize);
    int fn_getBatchSize() const;
    
    // Parallel processing management - ADD THESE
    void fn_setParallelProcessing(bool bEnable);
    bool fn_isParallelProcessing() const;
    
private:
    // Internal batch processing function - UPDATED to match implementation
    bool fn_internalBatchProcess(
        const std::vector<std::string>& vsFiles,
        const std::string& sOutputFormat,
        const std::string& sOutputDirectory,
        int iQuality,
        bool bPreserveMetadata,
        bool bVerbose
    );
    
    // Process single file in batch - UPDATED to match implementation
    bool fn_processSingleFile(
        const std::string& sInputFile,
        const std::string& sOutputFormat,
        const std::string& sOutputDirectory,
        int iQuality,
        bool bPreserveMetadata
    );
    
    // Helper functions - ADD THESE
    std::string fn_generateOutputFilename(
        const std::string& sInputFile,
        const std::string& sOutputFormat,
        const std::string& sOutputDirectory
    ) const;
    
    // FIXED: Changed from fn_validateOutputDirectory to fn_directoryExists
    bool fn_directoryExists(const std::string& sDirectory) const;
    
    // REMOVED: These are implemented as free functions in file_utils.cpp
    // bool fn_createDirectoryIfNeeded(const std::string& sDirectory);
    // bool fn_isHeicFile(const std::string& sFilePath);
    // std::vector<std::string> fn_collectDirectoryFiles(
    //     const std::string& sDirectory,
    //     bool bRecursive
    // );
    
    // Member variables
    int iProcessedCount;
    int iFailedCount;
    std::vector<std::string> vsFailedFiles;
    int iBatchSize;  // ADD THIS
    bool bParallelProcessing;  // ADD THIS
    
};

#endif  // BATCH_PROCESSOR_H