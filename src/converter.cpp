#include "converter.h"
#include "image_processor.h"
#include "format_encoder.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <sys/statvfs.h>
#include <algorithm>
#include <memory>

// Constructor
Converter::Converter()
{
    // Initialize members
    m_pLogger = std::make_shared<oLogger>();
    m_pImageProcessor = std::make_shared<ImageProcessor>(m_pLogger.get());
    m_pBatchProcessor = std::make_shared<BatchProcessor>();
} // End Constructor

// Destructor
Converter::~Converter()
{
    // Cleanup
    m_pImageProcessor.reset();
    m_pBatchProcessor.reset();
    m_pLogger.reset();
} // End Destructor

// Function: fn_initialize
int Converter::fn_initialize(const oConfig& oCurrentConfig)
{
    // Simple initialization
    m_pLogger->fn_logInfo("Converter initialized");
    return ERROR_SUCCESS;
} // End Function fn_initialize

// Function: fn_convertFile
int Converter::fn_convertFile(const std::string& sInputPath, 
                              const std::string& sOutputPath)
{
    m_pLogger->fn_logInfo("Converting: " + sInputPath + " to " + sOutputPath);
    
    // Check if input file exists
    if (!std::filesystem::exists(sInputPath)) {
        m_pLogger->fn_logError("Input file does not exist: " + sInputPath);
        return ERROR_FILE_NOT_FOUND;
    }
    
    // Check if output directory exists
    std::filesystem::path outputPath(sOutputPath);
    std::filesystem::path outputDir = outputPath.parent_path();
    
    if (!outputDir.empty() && !std::filesystem::exists(outputDir)) {
        if (!std::filesystem::create_directories(outputDir)) {
            m_pLogger->fn_logError("Failed to create output directory: " + outputDir.string());
            return ERROR_WRITE_PERMISSION;
        }
    }
    
    // Check if output file already exists
    if (std::filesystem::exists(sOutputPath)) {
        m_pLogger->fn_logWarning("Output file already exists: " + sOutputPath);
        // For now, we'll overwrite
    }
    
    // Extract format from output path
    std::string sOutputFormat = outputPath.extension().string();
    if (sOutputFormat.empty()) {
        sOutputFormat = ".jpg"; // Default format
    }
    
    // Remove the dot from format for image processor
    std::string formatWithoutDot = sOutputFormat;
    if (!formatWithoutDot.empty() && formatWithoutDot[0] == '.') {
        formatWithoutDot = formatWithoutDot.substr(1);
    }
    
    // Use ImageProcessor to convert the file
    bool success = m_pImageProcessor->fn_convertImage(
        sInputPath,
        sOutputPath,
        formatWithoutDot,
        85 // Default quality
    );
    
    if (!success) {
        m_pLogger->fn_logError("Conversion failed: " + sInputPath);
        return ERROR_DECODING_FAILED;
    }
    
    m_pLogger->fn_logSuccess("Successfully converted: " + sInputPath + " to " + sOutputPath);
    return ERROR_SUCCESS;
} // End Function fn_convertFile

// Set logger
void Converter::fn_setLogger(std::shared_ptr<oLogger> pLogger)
{
    m_pLogger = pLogger;
} // End Function fn_setLogger

// Get logger
std::shared_ptr<oLogger> Converter::fn_getLogger() const
{
    return m_pLogger;
} // End Function fn_getLogger

// Set image processor
void Converter::fn_setImageProcessor(std::shared_ptr<ImageProcessor> pProcessor)
{
    m_pImageProcessor = pProcessor;
} // End Function fn_setImageProcessor

// Set batch processor
void Converter::fn_setBatchProcessor(std::shared_ptr<BatchProcessor> pProcessor)
{
    m_pBatchProcessor = pProcessor;
} // End Function fn_setBatchProcessor

// Local Function: fn_isHeicFormat
bool Converter::fn_isHeicFormat(const std::string& sFilePath)
{
    std::string sExtension = std::filesystem::path(sFilePath).extension().string();
    std::transform(sExtension.begin(), sExtension.end(), sExtension.begin(), ::tolower);
    
    return (sExtension == ".heic" || sExtension == ".heif");
} // End Function fn_isHeicFormat

// Placeholder implementations for other functions
bool Converter::fn_convertSingleFile(const std::string& sInputPath, 
                                     const std::string& sOutputPath, 
                                     const ConversionOptions& oOptions)
{
    return fn_convertFile(sInputPath, sOutputPath) == ERROR_SUCCESS;
}

bool Converter::fn_convertBatch(const std::vector<std::string>& vsInputPaths, 
                               const std::string& sOutputDir, 
                               const ConversionOptions& oOptions)
{
    // TODO: Implement batch conversion
    return false;
}

bool Converter::fn_convertDirectory(const std::string& sInputDir, 
                                   const std::string& sOutputDir, 
                                   const ConversionOptions& oOptions)
{
    // TODO: Implement directory conversion
    return false;
}

bool Converter::fn_validateInputFile(const std::string& sFilePath)
{
    return std::filesystem::exists(sFilePath);
}

bool Converter::fn_validateOutputFormat(const std::string& sFormat)
{
    return true;
}

std::string Converter::fn_generateOutputPath(const std::string& sInputPath, 
                                            const std::string& sOutputDir, 
                                            const std::string& sFormat)
{
    std::filesystem::path inputPath(sInputPath);
    std::string stem = inputPath.stem().string();
    std::filesystem::path outputPath(sOutputDir);
    outputPath /= (stem + "." + sFormat);
    return outputPath.string();
}

bool Converter::fn_initializeCodecs()
{
    return true;
}

bool Converter::fn_cleanupTempFiles()
{
    return true;
}

bool Converter::fn_checkDiskSpace(const std::string& sPath, long long iRequiredBytes)
{
    return true;
}

bool Converter::fn_createDirectory(const std::string& sPath)
{
    return std::filesystem::create_directories(sPath);
}