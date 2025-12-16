// batch_processor.cpp - Batch processing implementation
// Author: R Square Innovation Software
// Version: v1.0

#include "batch_processor.h"
#include "converter.h"
#include "file_utils.h"
#include "logger.h"
#include <iostream>
#include <filesystem>
#include <thread>
#include <future>
#include <algorithm>

// Constructor - FIXED: Initialize all member variables
BatchProcessor::BatchProcessor()
{
    // Initialize member variables
    iProcessedCount = 0;
    iFailedCount = 0;
    iBatchSize = 10;  // Default batch size
    bParallelProcessing = true;  // Enable parallel by default
}  // End Constructor

// Destructor
BatchProcessor::~BatchProcessor()
{
    // Clear failed files vector
    vsFailedFiles.clear();
}  // End Destructor

// Process batch conversion - FIXED: Match function signature from header
bool BatchProcessor::fn_processBatch(
    const std::vector<std::string>& vsInputFiles,
    const std::string& sOutputFormat,
    const std::string& sOutputDirectory,
    int iQuality,
    bool bPreserveMetadata,
    bool bVerbose
)
{
    // Clear previous statistics
    fn_clearStatistics();
    
    // Validate output directory
    if (!fn_directoryExists(sOutputDirectory))
    {
        // Create directory if needed
        if (!fn_createDirectory(sOutputDirectory))
        {
            fn_logError("Failed to create output directory: " + sOutputDirectory);
            return false;
        }
    }
    
    // Process batch
    return fn_internalBatchProcess(
        vsInputFiles,
        sOutputFormat,
        sOutputDirectory,
        iQuality,
        bPreserveMetadata,
        bVerbose
    );
}  // End Function fn_processBatch

// Process directory conversion - FIXED: Match function signature from header
bool BatchProcessor::fn_processDirectory(
    const std::string& sInputDirectory,
    const std::string& sOutputFormat,
    const std::string& sOutputDirectory,
    bool bRecursive,
    int iQuality,
    bool bPreserveMetadata,
    bool bVerbose
)
{
    // Clear previous statistics
    fn_clearStatistics();
    
    // Validate input directory
    if (!std::filesystem::exists(sInputDirectory))
    {
        fn_logError("Input directory does not exist: " + sInputDirectory);
        return false;
    }
    
    // Validate output directory
    if (!fn_directoryExists(sOutputDirectory))
    {
        // Create directory if needed
        if (!fn_createDirectory(sOutputDirectory))
        {
            fn_logError("Failed to create output directory: " + sOutputDirectory);
            return false;
        }
    }
    
    // Collect files from directory
    std::vector<std::string> vsFiles = fn_collectDirectoryFiles(
        sInputDirectory,
        bRecursive
    );
    
    // Filter for HEIC/HEIF files
    std::vector<std::string> vsHeicFiles;
    
    for (const auto& sFile : vsFiles)
    {
        if (fn_isHeicFile(sFile))
        {
            vsHeicFiles.push_back(sFile);
        }
    }
    
    if (vsHeicFiles.empty())
    {
        fn_logWarning("No HEIC/HEIF files found in directory: " + sInputDirectory);
        return true;  // Nothing to process, not an error
    }
    
    if (bVerbose)
    {
        fn_logInfo("Found " + std::to_string(vsHeicFiles.size()) + " HEIC/HEIF files to process");
    }
    
    // Process batch
    return fn_internalBatchProcess(
        vsHeicFiles,
        sOutputFormat,
        sOutputDirectory,
        iQuality,
        bPreserveMetadata,
        bVerbose
    );
}  // End Function fn_processDirectory

// Get processed file count
int BatchProcessor::fn_getProcessedCount() const
{
    return iProcessedCount;
}  // End Function fn_getProcessedCount

// Get failed file count
int BatchProcessor::fn_getFailedCount() const
{
    return iFailedCount;
}  // End Function fn_getFailedCount

// Get list of failed files
std::vector<std::string> BatchProcessor::fn_getFailedFiles() const
{
    return vsFailedFiles;
}  // End Function fn_getFailedFiles

// Clear statistics
void BatchProcessor::fn_clearStatistics()
{
    iProcessedCount = 0;
    iFailedCount = 0;
    vsFailedFiles.clear();
}  // End Function fn_clearStatistics

// Set batch size - FIXED: Add implementation
void BatchProcessor::fn_setBatchSize(int iNewBatchSize)
{
    if (iNewBatchSize > 0)
    {
        iBatchSize = iNewBatchSize;
    }
    else
    {
        fn_logWarning("Invalid batch size, using default (10)");
    }
}  // End Function fn_setBatchSize

// Get current batch size - FIXED: Add implementation
int BatchProcessor::fn_getBatchSize() const
{
    return iBatchSize;
}  // End Function fn_getBatchSize

// Enable/disable parallel processing - FIXED: Add implementation
void BatchProcessor::fn_setParallelProcessing(bool bEnable)
{
    bParallelProcessing = bEnable;
}  // End Function fn_setParallelProcessing

// Check if parallel processing is enabled - FIXED: Add implementation
bool BatchProcessor::fn_isParallelProcessing() const
{
    return bParallelProcessing;
}  // End Function fn_isParallelProcessing

// Internal batch processing function - FIXED: Match function signature from header
bool BatchProcessor::fn_internalBatchProcess(
    const std::vector<std::string>& vsFiles,
    const std::string& sOutputFormat,
    const std::string& sOutputDirectory,
    int iQuality,
    bool bPreserveMetadata,
    bool bVerbose
)
{
    // Check if we have files to process
    if (vsFiles.empty())
    {
        fn_logWarning("No files to process");
        return true;  // Nothing to process, not an error
    }
    
    if (bVerbose)
    {
        fn_logInfo("Starting batch processing of " + std::to_string(vsFiles.size()) + " files");
    }
    
    // Process in batches
    size_t stTotalFiles = vsFiles.size();
    size_t stBatches = (stTotalFiles + iBatchSize - 1) / iBatchSize;
    
    for (size_t stBatch = 0; stBatch < stBatches; stBatch++)
    {
        // Calculate batch range
        size_t stStart = stBatch * iBatchSize;
        size_t stEnd = std::min(stStart + iBatchSize, stTotalFiles);
        
        if (bVerbose)
        {
            fn_logInfo("Processing batch " + std::to_string(stBatch + 1) + "/" + 
                       std::to_string(stBatches) + " (files " + 
                       std::to_string(stStart + 1) + "-" + 
                       std::to_string(stEnd) + ")");
        }
        
        // Process current batch
        if (bParallelProcessing)
        {
            // Parallel processing
            std::vector<std::future<bool>> vfResults;
            
            for (size_t stIdx = stStart; stIdx < stEnd; stIdx++)
            {
                vfResults.push_back(std::async(
                    std::launch::async,
                    [this, &vsFiles, stIdx, &sOutputFormat, &sOutputDirectory, iQuality, bPreserveMetadata]()
                    {
                        return fn_processSingleFile(
                            vsFiles[stIdx],
                            sOutputFormat,
                            sOutputDirectory,
                            iQuality,
                            bPreserveMetadata
                        );
                    }
                ));
            }
            
            // Collect results
            for (size_t stIdx = stStart; stIdx < stEnd; stIdx++)
            {
                bool bSuccess = vfResults[stIdx - stStart].get();
                
                if (bSuccess)
                {
                    iProcessedCount++;
                }
                else
                {
                    iFailedCount++;
                    vsFailedFiles.push_back(vsFiles[stIdx]);
                }
            }
        }
        else
        {
            // Sequential processing
            for (size_t stIdx = stStart; stIdx < stEnd; stIdx++)
            {
                bool bSuccess = fn_processSingleFile(
                    vsFiles[stIdx],
                    sOutputFormat,
                    sOutputDirectory,
                    iQuality,
                    bPreserveMetadata
                );
                
                if (bSuccess)
                {
                    iProcessedCount++;
                }
                else
                {
                    iFailedCount++;
                    vsFailedFiles.push_back(vsFiles[stIdx]);
                }
            }
        }
    }
    
    // Log summary
    fn_logInfo("Batch processing complete: " + 
               std::to_string(iProcessedCount) + " successful, " + 
               std::to_string(iFailedCount) + " failed");
    
    return iFailedCount == 0;  // Return true if no failures, false otherwise
}  // End Function fn_internalBatchProcess

// Process single file in batch - FIXED: Match function signature from header
bool BatchProcessor::fn_processSingleFile(
    const std::string& sInputFile,
    const std::string& sOutputFormat,
    const std::string& sOutputDirectory,
    int iQuality,
    bool bPreserveMetadata
)
{
    try
    {
        // Generate output filename
        std::string sOutputFile = fn_generateOutputFilename(
            sInputFile,
            sOutputFormat,
            sOutputDirectory
        );
        
        // Create converter instance
        Converter oConverter;
        
        // Configure converter - NOTE: These methods need to exist in Converter class
        // For now, we'll just try to convert
        int result = oConverter.fn_convertFile(sInputFile, sOutputFile);
        
        return (result == 0);  // Assuming 0 means success
        
    }
    catch (const std::exception& e)
    {
        fn_logError("Failed to process file " + sInputFile + ": " + e.what());
        return false;
    }
    catch (...)
    {
        fn_logError("Unknown error processing file: " + sInputFile);
        return false;
    }
}  // End Function fn_processSingleFile

// Generate output filename - FIXED: Add implementation
std::string BatchProcessor::fn_generateOutputFilename(
    const std::string& sInputFile,
    const std::string& sOutputFormat,
    const std::string& sOutputDirectory
) const
{
    // Get input filename without path
    std::filesystem::path oInputPath(sInputFile);
    std::string sFilename = oInputPath.stem().string();
    
    // Generate output filename with new extension
    std::string sOutputFilename = sFilename + "." + sOutputFormat;
    
    // Create full output path
    std::filesystem::path oOutputPath(sOutputDirectory);
    oOutputPath /= sOutputFilename;
    
    // Handle duplicate filenames
    int iCounter = 1;
    
    while (std::filesystem::exists(oOutputPath))
    {
        // Append counter to filename
        sOutputFilename = sFilename + "_" + std::to_string(iCounter) + "." + sOutputFormat;
        oOutputPath = std::filesystem::path(sOutputDirectory) / sOutputFilename;
        iCounter++;
    }
    
    return oOutputPath.string();
}  // End Function fn_generateOutputFilename

// Validate output directory - FIXED: Add implementation
bool BatchProcessor::fn_directoryExists(const std::string& sDirectory) const
{
    // Check if directory exists
    if (!std::filesystem::exists(sDirectory))
    {
        return false;
    }
    
    // Check if it's a directory
    if (!std::filesystem::is_directory(sDirectory))
    {
        return false;
    }
    
    // Check write permissions (simplified check)
    std::filesystem::path oTestPath = std::filesystem::path(sDirectory) / ".write_test";
    
    try
    {
        // Try to create a test file
        std::ofstream oTestFile(oTestPath.string());
        
        if (!oTestFile.is_open())
        {
            return false;
        }
        
        oTestFile.close();
        std::filesystem::remove(oTestPath);
        
        return true;
    }
    catch (...)
    {
        return false;
    }
}  // End Function fn_directoryExists
