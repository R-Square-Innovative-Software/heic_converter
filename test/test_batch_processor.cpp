// test_batch_processor.cpp - Unit tests for batch processor
// Author: R Square Innovation Software
// Version: v1.0

#include "batch_processor.h"
#include "config.h"
#include "converter.h"
#include "logger.h"
#include "file_utils.h"
#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>

// Test Fixture for Batch Processor
class BatchProcessorTest : public ::testing::Test 
{ // Begin class BatchProcessorTest
protected:
    void SetUp() override 
    { // Begin SetUp
        sTestInputDir = "test_input_dir"; // Local Function
        sTestOutputDir = "test_output_dir"; // Local Function
        
        // Create test directories
        std::filesystem::create_directory(sTestInputDir); // In filesystem
        std::filesystem::create_directory(sTestOutputDir); // In filesystem
        
        // Create test files
        fn_createTestFiles(); // Local Function
        
        // Setup default config
        oTestConfig = fn_getDefaultConfig(); // In config.cpp
        oTestConfig.sInputPath = sTestInputDir; // Local Function
        oTestConfig.sOutputPath = sTestOutputDir; // Local Function
        oTestConfig.bVerbose = false; // Local Function
        
        // Create mock converter
        oMockConverter = std::make_shared<MockConverter>(); // Local Function
    } // End SetUp
    
    void TearDown() override 
    { // Begin TearDown
        // Clean up test directories
        if (std::filesystem::exists(sTestInputDir)) 
        { // Begin if
            std::filesystem::remove_all(sTestInputDir); // In filesystem
        } // End if(std::filesystem::exists(sTestInputDir))
        
        if (std::filesystem::exists(sTestOutputDir)) 
        { // Begin if
            std::filesystem::remove_all(sTestOutputDir); // In filesystem
        } // End if(std::filesystem::exists(sTestOutputDir))
    } // End TearDown
    
    // Local Function
    void fn_createTestFiles() 
    { // Begin fn_createTestFiles
        // Create HEIC files (simulate with text files for testing)
        std::vector<std::string> vsTestFiles = { // Local Function
            "image1.heic",
            "image2.heif",
            "image3.HEIC",
            "image4.HEIF",
            "not_an_image.txt",
            "image5.jpg",  // Not a HEIC file
            "subdir/image6.heic",
            "subdir/image7.heif"
        };
        
        for (const auto& sFileName : vsTestFiles) 
        { // Begin for
            std::filesystem::path oFilePath = sTestInputDir + "/" + sFileName; // In filesystem
            
            // Create parent directory if needed
            std::filesystem::create_directories(oFilePath.parent_path()); // In filesystem
            
            // Create file with some content
            std::ofstream oFile(oFilePath); // In fstream
            oFile << "Test content for " << sFileName; // In fstream
            oFile.close(); // In fstream
        } // End for(const auto& sFileName : vsTestFiles)
    } // End Function fn_createTestFiles
    
    // Mock Converter for testing
    class MockConverter : public oConverter 
    { // Begin class MockConverter
    public:
        int fn_convertFile(const std::string& sInputPath, const std::string& sOutputPath) override 
        { // Begin fn_convertFile
            vConvertedFiles.push_back({sInputPath, sOutputPath}); // Local Function
            
            // Simulate successful conversion
            if (sInputPath.find("error") != std::string::npos) 
            { // Begin if
                return ERROR_DECODING_FAILED; // Simulate error
            } // End if(sInputPath.find("error") != std::string::npos)
            
            // Create output file
            std::ofstream oFile(sOutputPath); // In fstream
            oFile << "Converted: " << sInputPath; // In fstream
            oFile.close(); // In fstream
            
            return ERROR_SUCCESS; // Success
        } // End Function fn_convertFile
        
        std::vector<std::pair<std::string, std::string>> vConvertedFiles; // Local Function
    }; // End class MockConverter
    
    std::string sTestInputDir; // Local Function
    std::string sTestOutputDir; // Local Function
    oConfig oTestConfig; // Local Function
    std::shared_ptr<MockConverter> oMockConverter; // Local Function
}; // End class BatchProcessorTest

// Test Case: Process Directory Successfully
TEST_F(BatchProcessorTest, ProcessDirectorySuccess) 
{ // Begin TEST_F
    oBatchProcessor oBatch; // In batch_processor.h
    
    // Process directory
    int iResult = oBatch.fn_processDirectory( // In batch_processor.cpp
        sTestInputDir, // Local Function
        sTestOutputDir, // Local Function
        oTestConfig, // Local Function
        *oMockConverter // Local Function
    );
    
    EXPECT_EQ(iResult, ERROR_SUCCESS); // Should succeed
    
    // Check that output directory exists
    EXPECT_TRUE(std::filesystem::exists(sTestOutputDir)); // In filesystem
    
    // Check that HEIC files were processed (excluding txt and jpg files)
    EXPECT_GE(oMockConverter->vConvertedFiles.size(), 4); // Should process at least 4 HEIC files
} // End TEST_F(ProcessDirectorySuccess)

// Test Case: Process Directory Recursively
TEST_F(BatchProcessorTest, ProcessDirectoryRecursive) 
{ // Begin TEST_F
    oBatchProcessor oBatch; // In batch_processor.h
    
    // Enable recursive processing
    oTestConfig.bRecursive = true; // Local Function
    
    int iResult = oBatch.fn_processDirectory( // In batch_processor.cpp
        sTestInputDir, // Local Function
        sTestOutputDir, // Local Function
        oTestConfig, // Local Function
        *oMockConverter // Local Function
    );
    
    EXPECT_EQ(iResult, ERROR_SUCCESS); // Should succeed
    
    // Should process files in subdirectory too
    bool bFoundSubdirFile = false; // Local Function
    for (const auto& oPair : oMockConverter->vConvertedFiles) 
    { // Begin for
        if (oPair.first.find("subdir") != std::string::npos) 
        { // Begin if
            bFoundSubdirFile = true; // Local Function
            break; // Found subdirectory file
        } // End if(oPair.first.find("subdir") != std::string::npos)
    } // End for(const auto& oPair : oMockConverter->vConvertedFiles)
    
    EXPECT_TRUE(bFoundSubdirFile); // Should process files in subdirectory
} // End TEST_F(ProcessDirectoryRecursive)

// Test Case: Non-Recursive Processing
TEST_F(BatchProcessorTest, ProcessDirectoryNonRecursive) 
{ // Begin TEST_F
    oBatchProcessor oBatch; // In batch_processor.h
    
    // Disable recursive processing
    oTestConfig.bRecursive = false; // Local Function
    
    int iResult = oBatch.fn_processDirectory( // In batch_processor.cpp
        sTestInputDir, // Local Function
        sTestOutputDir, // Local Function
        oTestConfig, // Local Function
        *oMockConverter // Local Function
    );
    
    EXPECT_EQ(iResult, ERROR_SUCCESS); // Should succeed
    
    // Should NOT process files in subdirectory
    bool bFoundSubdirFile = false; // Local Function
    for (const auto& oPair : oMockConverter->vConvertedFiles) 
    { // Begin for
        if (oPair.first.find("subdir") != std::string::npos) 
        { // Begin if
            bFoundSubdirFile = true; // Local Function
            break; // Found subdirectory file
        } // End if(oPair.first.find("subdir") != std::string::npos)
    } // End for(const auto& oPair : oMockConverter->vConvertedFiles)
    
    EXPECT_FALSE(bFoundSubdirFile); // Should NOT process files in subdirectory
} // End TEST_F(ProcessDirectoryNonRecursive)

// Test Case: Thread Count Configuration
TEST_F(BatchProcessorTest, ProcessDirectoryWithThreads) 
{ // Begin TEST_F
    oBatchProcessor oBatch; // In batch_processor.h
    
    // Test with 2 threads
    oTestConfig.iThreadCount = 2; // Local Function
    
    int iResult = oBatch.fn_processDirectory( // In batch_processor.cpp
        sTestInputDir, // Local Function
        sTestOutputDir, // Local Function
        oTestConfig, // Local Function
        *oMockConverter // Local Function
    );
    
    EXPECT_EQ(iResult, ERROR_SUCCESS); // Should succeed with thread pool
} // End TEST_F(ProcessDirectoryWithThreads)

// Test Case: Overwrite Existing Files
TEST_F(BatchProcessorTest, ProcessDirectoryWithOverwrite) 
{ // Begin TEST_F
    oBatchProcessor oBatch; // In batch_processor.h
    
    // Create existing output files
    std::filesystem::create_directories(sTestOutputDir); // In filesystem
    std::ofstream oExistingFile(sTestOutputDir + "/image1.jpg"); // In fstream
    oExistingFile << "Existing content"; // In fstream
    oExistingFile.close(); // In fstream
    
    // Enable overwrite
    oTestConfig.bOverwrite = true; // Local Function
    
    int iResult = oBatch.fn_processDirectory( // In batch_processor.cpp
        sTestInputDir, // Local Function
        sTestOutputDir, // Local Function
        oTestConfig, // Local Function
        *oMockConverter // Local Function
    );
    
    EXPECT_EQ(iResult, ERROR_SUCCESS); // Should succeed and overwrite
} // End TEST_F(ProcessDirectoryWithOverwrite)

// Test Case: Skip Existing Files (No Overwrite)
TEST_F(BatchProcessorTest, ProcessDirectorySkipExisting) 
{ // Begin TEST_F
    oBatchProcessor oBatch; // In batch_processor.h
    
    // Create existing output files
    std::filesystem::create_directories(sTestOutputDir); // In filesystem
    std::ofstream oExistingFile(sTestOutputDir + "/image1.jpg"); // In fstream
    oExistingFile << "Existing content"; // In fstream
    oExistingFile.close(); // In fstream
    
    // Disable overwrite (default)
    oTestConfig.bOverwrite = false; // Local Function
    
    int iResult = oBatch.fn_processDirectory( // In batch_processor.cpp
        sTestInputDir, // Local Function
        sTestOutputDir, // Local Function
        oTestConfig, // Local Function
        *oMockConverter // Local Function
    );
    
    EXPECT_EQ(iResult, ERROR_SUCCESS); // Should succeed but skip existing
    
    // Check that existing file was not modified
    std::ifstream oFile(sTestOutputDir + "/image1.jpg"); // In fstream
    std::string sContent; // Local Function
    std::getline(oFile, sContent); // In fstream
    oFile.close(); // In fstream
    
    EXPECT_EQ(sContent, "Existing content"); // Content should not be changed
} // End TEST_F(ProcessDirectorySkipExisting)

// Test Case: Input Directory Does Not Exist
TEST_F(BatchProcessorTest, ProcessDirectoryNotFound) 
{ // Begin TEST_F
    oBatchProcessor oBatch; // In batch_processor.h
    
    int iResult = oBatch.fn_processDirectory( // In batch_processor.cpp
        "non_existent_directory", // Local Function
        sTestOutputDir, // Local Function
        oTestConfig, // Local Function
        *oMockConverter // Local Function
    );
    
    EXPECT_EQ(iResult, ERROR_FILE_NOT_FOUND); // Should fail with file not found
} // End TEST_F(ProcessDirectoryNotFound)

// Test Case: Output Directory Creation
TEST_F(BatchProcessorTest, ProcessDirectoryCreateOutput) 
{ // Begin TEST_F
    oBatchProcessor oBatch; // In batch_processor.h
    
    // Remove output directory
    std::filesystem::remove_all(sTestOutputDir); // In filesystem
    
    int iResult = oBatch.fn_processDirectory( // In batch_processor.cpp
        sTestInputDir, // Local Function
        sTestOutputDir, // Local Function
        oTestConfig, // Local Function
        *oMockConverter // Local Function
    );
    
    EXPECT_EQ(iResult, ERROR_SUCCESS); // Should succeed and create directory
    EXPECT_TRUE(std::filesystem::exists(sTestOutputDir)); // In filesystem
} // End TEST_F(ProcessDirectoryCreateOutput)

// Test Case: Empty Input Directory
TEST_F(BatchProcessorTest, ProcessEmptyDirectory) 
{ // Begin TEST_F
    oBatchProcessor oBatch; // In batch_processor.h
    
    // Create empty directory
    std::string sEmptyDir = "test_empty_dir"; // Local Function
    std::filesystem::create_directory(sEmptyDir); // In filesystem
    
    int iResult = oBatch.fn_processDirectory( // In batch_processor.cpp
        sEmptyDir, // Local Function
        sTestOutputDir, // Local Function
        oTestConfig, // Local Function
        *oMockConverter // Local Function
    );
    
    // Clean up empty directory
    std::filesystem::remove_all(sEmptyDir); // In filesystem
    
    EXPECT_EQ(iResult, ERROR_SUCCESS); // Should succeed (no files to process)
    EXPECT_EQ(oMockConverter->vConvertedFiles.size(), 0); // No files converted
} // End TEST_F(ProcessEmptyDirectory)

// Test Case: File Filtering
TEST_F(BatchProcessorTest, ProcessDirectoryFileFilter) 
{ // Begin TEST_F
    oBatchProcessor oBatch; // In batch_processor.h
    
    int iResult = oBatch.fn_processDirectory( // In batch_processor.cpp
        sTestInputDir, // Local Function
        sTestOutputDir, // Local Function
        oTestConfig, // Local Function
        *oMockConverter // Local Function
    );
    
    EXPECT_EQ(iResult, ERROR_SUCCESS); // Should succeed
    
    // Check that only HEIC/HEIF files were processed
    for (const auto& oPair : oMockConverter->vConvertedFiles) 
    { // Begin for
        std::string sExtension = fn_getFileExtension(oPair.first); // In file_utils.cpp
        EXPECT_TRUE(fn_isSupportedInputFormat(sExtension)); // In config.cpp
    } // End for(const auto& oPair : oMockConverter->vConvertedFiles)
} // End TEST_F(ProcessDirectoryFileFilter)

// Test Case: Output Path Generation
TEST_F(BatchProcessorTest, ProcessDirectoryOutputPaths) 
{ // Begin TEST_F
    oBatchProcessor oBatch; // In batch_processor.h
    
    // Set output format
    oTestConfig.sOutputFormat = ".png"; // Local Function
    
    int iResult = oBatch.fn_processDirectory( // In batch_processor.cpp
        sTestInputDir, // Local Function
        sTestOutputDir, // Local Function
        oTestConfig, // Local Function
        *oMockConverter // Local Function
    );
    
    EXPECT_EQ(iResult, ERROR_SUCCESS); // Should succeed
    
    // Check output file extensions
    for (const auto& oPair : oMockConverter->vConvertedFiles) 
    { // Begin for
        std::string sOutputExtension = fn_getFileExtension(oPair.second); // In file_utils.cpp
        EXPECT_EQ(sOutputExtension, ".png"); // Should be .png
    } // End for(const auto& oPair : oMockConverter->vConvertedFiles)
} // End TEST_F(ProcessDirectoryOutputPaths)

// Test Case: Batch Processing with Conversion Errors
TEST_F(BatchProcessorTest, ProcessDirectoryWithErrors) 
{ // Begin TEST_F
    // Create a file that will cause conversion error
    std::string sErrorFile = sTestInputDir + "/error.heic"; // Local Function
    std::ofstream oErrorFile(sErrorFile); // In fstream
    oErrorFile << "This will cause an error"; // In fstream
    oErrorFile.close(); // In fstream
    
    oBatchProcessor oBatch; // In batch_processor.h
    
    int iResult = oBatch.fn_processDirectory( // In batch_processor.cpp
        sTestInputDir, // Local Function
        sTestOutputDir, // Local Function
        oTestConfig, // Local Function
        *oMockConverter // Local Function
    );
    
    // Even with errors, batch processor should continue processing other files
    EXPECT_EQ(iResult, ERROR_SUCCESS); // Should report success (errors logged but continue)
    
    // Should still process other files
    EXPECT_GT(oMockConverter->vConvertedFiles.size(), 0); // Should process some files
} // End TEST_F(ProcessDirectoryWithErrors)

// Test Case: Batch Progress Reporting
TEST_F(BatchProcessorTest, ProcessDirectoryProgress) 
{ // Begin TEST_F
    oBatchProcessor oBatch; // In batch_processor.h
    
    // Enable verbose mode
    oTestConfig.bVerbose = true; // Local Function
    
    int iResult = oBatch.fn_processDirectory( // In batch_processor.cpp
        sTestInputDir, // Local Function
        sTestOutputDir, // Local Function
        oTestConfig, // Local Function
        *oMockConverter // Local Function
    );
    
    EXPECT_EQ(iResult, ERROR_SUCCESS); // Should succeed
    // Note: In verbose mode, progress should be logged
} // End TEST_F(ProcessDirectoryProgress)

// Main function for running tests
int main(int argc, char** argv) 
{ // Begin main
    ::testing::InitGoogleTest(&argc, argv); // In gtest
    return RUN_ALL_TESTS(); // In gtest
} // End Function main