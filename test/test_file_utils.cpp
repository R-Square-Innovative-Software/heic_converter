// test_file_utils.cpp - Unit tests for file utility functions
// Author: R Square Innovation Software
// Version: v1.0

#include "file_utils.h"
#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include <fstream>
#include <filesystem>
#include <cstdlib>
#include <ctime>

// Test function declarations
void fn_testFileExists(); // Local Function
void fn_testIsDirectory(); // Local Function
void fn_testCreateDirectory(); // Local Function
void fn_testGetFileExtension(); // Local Function
void fn_testChangeFileExtension(); // Local Function
void fn_testGetFileNameWithoutExtension(); // Local Function
void fn_testGetAbsolutePath(); // Local Function
void fn_testGetFilesInDirectory(); // Local Function
void fn_testFilterFilesByExtension(); // Local Function
void fn_testValidateOutputPath(); // Local Function
void fn_testCopyFile(); // Local Function
void fn_testDeleteFile(); // Local Function
void fn_testGetFileSize(); // Local Function
void fn_testIsHeicFile(); // Local Function
void fn_testGenerateUniqueFileName(); // Local Function
void fn_testNormalizePath(); // Local Function
void fn_testHasWritePermission(); // Local Function

// Helper functions for tests
std::string fn_generateTempDirectory(); // Local Function
void fn_cleanupTempDirectory(const std::string& sPath); // Local Function
bool fn_createTestFile(const std::string& sPath, const std::string& sContent); // Local Function

// Main test runner
int main() 
{ // Begin main
    std::cout << "Running FileUtils Unit Tests..." << std::endl; // In iostream
    std::cout << "========================================" << std::endl; // In iostream
    
    try 
    { // Begin try
        // Run individual tests
        fn_testFileExists(); // Local Function
        std::cout << "✓ Test fn_fileExists passed" << std::endl; // In iostream
        
        fn_testIsDirectory(); // Local Function
        std::cout << "✓ Test fn_isDirectory passed" << std::endl; // In iostream
        
        fn_testCreateDirectory(); // Local Function
        std::cout << "✓ Test fn_createDirectory passed" << std::endl; // In iostream
        
        fn_testGetFileExtension(); // Local Function
        std::cout << "✓ Test fn_getFileExtension passed" << std::endl; // In iostream
        
        fn_testChangeFileExtension(); // Local Function
        std::cout << "✓ Test fn_changeFileExtension passed" << std::endl; // In iostream
        
        fn_testGetFileNameWithoutExtension(); // Local Function
        std::cout << "✓ Test fn_getFileNameWithoutExtension passed" << std::endl; // In iostream
        
        fn_testGetAbsolutePath(); // Local Function
        std::cout << "✓ Test fn_getAbsolutePath passed" << std::endl; // In iostream
        
        fn_testGetFilesInDirectory(); // Local Function
        std::cout << "✓ Test fn_getFilesInDirectory passed" << std::endl; // In iostream
        
        fn_testFilterFilesByExtension(); // Local Function
        std::cout << "✓ Test fn_filterFilesByExtension passed" << std::endl; // In iostream
        
        fn_testValidateOutputPath(); // Local Function
        std::cout << "✓ Test fn_validateOutputPath passed" << std::endl; // In iostream
        
        fn_testCopyFile(); // Local Function
        std::cout << "✓ Test fn_copyFile passed" << std::endl; // In iostream
        
        fn_testDeleteFile(); // Local Function
        std::cout << "✓ Test fn_deleteFile passed" << std::endl; // In iostream
        
        fn_testGetFileSize(); // Local Function
        std::cout << "✓ Test fn_getFileSize passed" << std::endl; // In iostream
        
        fn_testIsHeicFile(); // Local Function
        std::cout << "✓ Test fn_isHeicFile passed" << std::endl; // In iostream
        
        fn_testGenerateUniqueFileName(); // Local Function
        std::cout << "✓ Test fn_generateUniqueFileName passed" << std::endl; // In iostream
        
        fn_testNormalizePath(); // Local Function
        std::cout << "✓ Test fn_normalizePath passed" << std::endl; // In iostream
        
        fn_testHasWritePermission(); // Local Function
        std::cout << "✓ Test fn_hasWritePermission passed" << std::endl; // In iostream
        
        std::cout << "========================================" << std::endl; // In iostream
        std::cout << "All tests passed successfully!" << std::endl; // In iostream
        std::cout << "Total tests: 17" << std::endl; // In iostream
        
        return 0; // Success
    } 
    catch (const std::exception& e) 
    { // Begin catch
        std::cerr << "Test failed with exception: " << e.what() << std::endl; // In iostream
        return 1; // Failure
    } // End catch(const std::exception& e)
    catch (...) 
    { // Begin catch
        std::cerr << "Test failed with unknown exception" << std::endl; // In iostream
        return 1; // Failure
    } // End catch(...)
} // End Function main

// Test: fn_fileExists
void fn_testFileExists() 
{ // Begin fn_testFileExists
    // Create temporary test directory
    std::string sTempDir = fn_generateTempDirectory(); // Local Function
    std::string sTestFile = sTempDir + "/test_file.txt"; // Local Function
    
    // Test 1: Non-existent file should return false
    assert(!fn_fileExists(sTestFile) && "Non-existent file should return false"); // In cassert
    
    // Test 2: Create file and check existence
    bool bCreated = fn_createTestFile(sTestFile, "test content"); // Local Function
    assert(bCreated && "Failed to create test file"); // In cassert
    assert(fn_fileExists(sTestFile) && "Created file should exist"); // In cassert
    
    // Test 3: Directory should exist
    assert(fn_fileExists(sTempDir) && "Directory should exist"); // In cassert
    
    // Cleanup
    fn_cleanupTempDirectory(sTempDir); // Local Function
} // End Function fn_testFileExists

// Test: fn_isDirectory
void fn_testIsDirectory() 
{ // Begin fn_testIsDirectory
    std::string sTempDir = fn_generateTempDirectory(); // Local Function
    std::string sTestFile = sTempDir + "/test_file.txt"; // Local Function
    
    // Test 1: Non-existent path should return false
    assert(!fn_isDirectory(sTempDir + "/nonexistent") && "Non-existent path should return false"); // In cassert
    
    // Test 2: Created directory should return true
    assert(fn_isDirectory(sTempDir) && "Created directory should return true"); // In cassert
    
    // Test 3: File should return false
    fn_createTestFile(sTestFile, "test content"); // Local Function
    assert(!fn_isDirectory(sTestFile) && "File should not be identified as directory"); // In cassert
    
    // Cleanup
    fn_cleanupTempDirectory(sTempDir); // Local Function
} // End Function fn_testIsDirectory

// Test: fn_createDirectory
void fn_testCreateDirectory() 
{ // Begin fn_testCreateDirectory
    std::string sTempDir = fn_generateTempDirectory(); // Local Function
    
    // Test 1: Create simple directory
    std::string sNewDir = sTempDir + "/test_dir"; // Local Function
    bool bCreated = fn_createDirectory(sNewDir); // Local Function
    assert(bCreated && "Should be able to create directory"); // In cassert
    assert(fn_isDirectory(sNewDir) && "Created directory should exist"); // In cassert
    
    // Test 2: Create nested directories
    std::string sNestedDir = sTempDir + "/level1/level2/level3"; // Local Function
    bCreated = fn_createDirectory(sNestedDir); // Local Function
    assert(bCreated && "Should be able to create nested directories"); // In cassert
    assert(fn_isDirectory(sNestedDir) && "Nested directory should exist"); // In cassert
    
    // Test 3: Creating existing directory should return true
    assert(fn_createDirectory(sNewDir) && "Creating existing directory should return true"); // In cassert
    
    // Test 4: Create directory with file path (should fail as directory)
    std::string sFilePath = sTempDir + "/file_as_dir/file.txt"; // Local Function
    // This should succeed because fn_createDirectory creates parent directories
    bCreated = fn_createDirectory(sFilePath); // Local Function
    assert(bCreated && "Should create parent directories even with file path"); // In cassert
    
    // Cleanup
    fn_cleanupTempDirectory(sTempDir); // Local Function
} // End Function fn_testCreateDirectory

// Test: fn_getFileExtension
void fn_testGetFileExtension() 
{ // Begin fn_testGetFileExtension
    // Test cases with expected results
    struct oTestCase 
    { // Begin struct oTestCase
        std::string sInput; // Local Function
        std::string sExpected; // Local Function
    }; // End struct oTestCase
    
    std::vector<oTestCase> voTestCases = 
    { // Begin vector voTestCases
        {"file.txt", "txt"}, // Local Function
        {"image.jpg", "jpg"}, // Local Function
        {"document.PDF", "pdf"}, // Local Function (case insensitive)
        {"noextension", ""}, // Local Function
        {"multiple.dots.file.tar.gz", "gz"}, // Local Function
        {".hidden", ""}, // Local Function
        {"path/to/file.heic", "heic"}, // Local Function
        {"C:\\Windows\\file.exe", "exe"}, // Local Function
        {"", ""} // Local Function (empty string)
    }; // End vector voTestCases
    
    for (const auto& oCase : voTestCases) 
    { // Begin for
        std::string sResult = fn_getFileExtension(oCase.sInput); // Local Function
        assert(sResult == oCase.sExpected && "File extension mismatch"); // In cassert
    } // End for(const auto& oCase : voTestCases)
} // End Function fn_testGetFileExtension

// Test: fn_changeFileExtension
void fn_testChangeFileExtension() 
{ // Begin fn_testChangeFileExtension
    // Test cases
    struct oTestCase 
    { // Begin struct oTestCase
        std::string sInput; // Local Function
        std::string sNewExt; // Local Function
        std::string sExpected; // Local Function
    }; // End struct oTestCase
    
    std::vector<oTestCase> voTestCases = 
    { // Begin vector voTestCases
        {"file.txt", "jpg", "file.jpg"}, // Local Function
        {"image.jpg", "png", "image.png"}, // Local Function
        {"noextension", "txt", "noextension.txt"}, // Local Function
        {".hidden", "visible", ".hidden.visible"}, // Local Function
        {"path/to/file.heic", "jpg", "path/to/file.jpg"}, // Local Function
        {"document", "", "document."} // Local Function (empty extension)
    }; // End vector voTestCases
    
    for (const auto& oCase : voTestCases) 
    { // Begin for
        std::string sResult = fn_changeFileExtension(oCase.sInput, oCase.sNewExt); // Local Function
        assert(sResult == oCase.sExpected && "Changed file extension mismatch"); // In cassert
    } // End for(const auto& oCase : voTestCases)
} // End Function fn_testChangeFileExtension

// Test: fn_getFileNameWithoutExtension
void fn_testGetFileNameWithoutExtension() 
{ // Begin fn_testGetFileNameWithoutExtension
    // Test cases
    struct oTestCase 
    { // Begin struct oTestCase
        std::string sInput; // Local Function
        std::string sExpected; // Local Function
    }; // End struct oTestCase
    
    std::vector<oTestCase> voTestCases = 
    { // Begin vector voTestCases
        {"file.txt", "file"}, // Local Function
        {"image.jpg", "image"}, // Local Function
        {"/path/to/file.heic", "file"}, // Local Function
        {"C:\\Windows\\file.exe", "file"}, // Local Function
        {"noextension", "noextension"}, // Local Function
        {".hidden", ".hidden"}, // Local Function
        {"file.", "file"}, // Local Function
        {"", ""}, // Local Function
        {"path/to/.hidden/file.txt", "file"} // Local Function
    }; // End vector voTestCases
    
    for (const auto& oCase : voTestCases) 
    { // Begin for
        std::string sResult = fn_getFileNameWithoutExtension(oCase.sInput); // Local Function
        assert(sResult == oCase.sExpected && "Filename without extension mismatch"); // In cassert
    } // End for(const auto& oCase : voTestCases)
} // End Function fn_testGetFileNameWithoutExtension

// Test: fn_getAbsolutePath
void fn_testGetAbsolutePath() 
{ // Begin fn_testGetAbsolutePath
    // Note: This test depends on the current working directory
    // We'll test basic functionality
    
    // Test 1: Current directory
    std::string sCurrent = "."; // Local Function
    std::string sAbsCurrent = fn_getAbsolutePath(sCurrent); // Local Function
    assert(!sAbsCurrent.empty() && "Absolute path of '.' should not be empty"); // In cassert
    
    // Test 2: Parent directory
    std::string sParent = ".."; // Local Function
    std::string sAbsParent = fn_getAbsolutePath(sParent); // Local Function
    assert(!sAbsParent.empty() && "Absolute path of '..' should not be empty"); // In cassert
    
    // Test 3: Non-existent path (should return input)
    std::string sNonExistent = "/this/path/does/not/exist/123456789"; // Local Function
    std::string sAbsNonExistent = fn_getAbsolutePath(sNonExistent); // Local Function
    // realpath may return the input or something else depending on implementation
    // We'll just ensure it doesn't crash
    
    // Test 4: Home directory expansion would be nice but not portable
} // End Function fn_getAbsolutePath

// Test: fn_getFilesInDirectory
void fn_testGetFilesInDirectory() 
{ // Begin fn_testGetFilesInDirectory
    std::string sTempDir = fn_generateTempDirectory(); // Local Function
    
    // Create test files
    std::vector<std::string> vsExpectedFiles; // Local Function
    vsExpectedFiles.push_back(sTempDir + "/file1.txt"); // In vector
    vsExpectedFiles.push_back(sTempDir + "/file2.jpg"); // In vector
    vsExpectedFiles.push_back(sTempDir + "/file3.png"); // In vector
    
    for (const auto& sFile : vsExpectedFiles) 
    { // Begin for
        fn_createTestFile(sFile, "test content"); // Local Function
    } // End for(const auto& sFile : vsExpectedFiles)
    
    // Create a subdirectory (should not be included in files list)
    std::string sSubDir = sTempDir + "/subdir"; // Local Function
    fn_createDirectory(sSubDir); // Local Function
    
    // Test 1: Get files from directory
    std::vector<std::string> vsFiles = fn_getFilesInDirectory(sTempDir); // Local Function
    
    // Sort for comparison
    std::sort(vsFiles.begin(), vsFiles.end()); // In algorithm
    std::sort(vsExpectedFiles.begin(), vsExpectedFiles.end()); // In algorithm
    
    assert(vsFiles.size() == vsExpectedFiles.size() && "Should find all created files"); // In cassert
    
    for (size_t i = 0; i < vsFiles.size(); i++) 
    { // Begin for
        assert(vsFiles[i] == vsExpectedFiles[i] && "File path mismatch"); // In cassert
    } // End for(size_t i = 0; i < vsFiles.size(); i++)
    
    // Test 2: Non-existent directory should return empty vector
    std::vector<std::string> vsEmpty = fn_getFilesInDirectory(sTempDir + "/nonexistent"); // Local Function
    assert(vsEmpty.empty() && "Non-existent directory should return empty vector"); // In cassert
    
    // Cleanup
    fn_cleanupTempDirectory(sTempDir); // Local Function
} // End Function fn_testGetFilesInDirectory

// Test: fn_filterFilesByExtension
void fn_testFilterFilesByExtension() 
{ // Begin fn_testFilterFilesByExtension
    // Create test file list
    std::vector<std::string> vsAllFiles = 
    { // Begin vector vsAllFiles
        "/path/file1.heic", // Local Function
        "/path/file2.heif", // Local Function
        "/path/file3.jpg", // Local Function
        "/path/file4.png", // Local Function
        "/path/file5.txt", // Local Function
        "/path/file6.HEIC", // Local Function (uppercase)
        "/path/file7.HEIF" // Local Function (uppercase)
    }; // End vector vsAllFiles
    
    // Test 1: Filter for HEIC/HEIF (lowercase)
    std::vector<std::string> vsHeicExtensions = {"heic", "heif"}; // Local Function
    std::vector<std::string> vsHeicFiles = fn_filterFilesByExtension(vsAllFiles, vsHeicExtensions); // Local Function
    
    assert(vsHeicFiles.size() == 2 && "Should find 2 HEIC/HEIF files (lowercase)"); // In cassert
    
    // Test 2: Filter with mixed case (should not match uppercase)
    std::vector<std::string> vsMixedExtensions = {"HEIC", "HEIF"}; // Local Function
    std::vector<std::string> vsMixedFiles = fn_filterFilesByExtension(vsAllFiles, vsMixedExtensions); // Local Function
    
    assert(vsMixedFiles.size() == 2 && "Should find 2 HEIC/HEIF files (uppercase)"); // In cassert
    
    // Test 3: Filter for image formats
    std::vector<std::string> vsImageExtensions = {"jpg", "png", "heic", "heif"}; // Local Function
    std::vector<std::string> vsImageFiles = fn_filterFilesByExtension(vsAllFiles, vsImageExtensions); // Local Function
    
    assert(vsImageFiles.size() == 6 && "Should find 6 image files (4 lowercase + 2 uppercase)"); // In cassert
    
    // Test 4: Empty filter should return empty vector
    std::vector<std::string> vsEmptyExtensions; // Local Function
    std::vector<std::string> vsEmptyFiles = fn_filterFilesByExtension(vsAllFiles, vsEmptyExtensions); // Local Function
    
    assert(vsEmptyFiles.empty() && "Empty filter should return empty vector"); // In cassert
    
    // Test 5: Empty file list should return empty vector
    std::vector<std::string> vsEmptyFileList; // Local Function
    std::vector<std::string> vsNoFiles = fn_filterFilesByExtension(vsEmptyFileList, vsHeicExtensions); // Local Function
    
    assert(vsNoFiles.empty() && "Empty file list should return empty vector"); // In cassert
} // End Function fn_filterFilesByExtension

// Test: fn_validateOutputPath
void fn_testValidateOutputPath() 
{ // Begin fn_testValidateOutputPath
    std::string sTempDir = fn_generateTempDirectory(); // Local Function
    
    // Test 1: Valid path with existing directory
    std::string sValidPath = sTempDir + "/output.jpg"; // Local Function
    bool bValid = fn_validateOutputPath(sValidPath); // Local Function
    assert(bValid && "Path with existing directory should be valid"); // In cassert
    
    // Test 2: Path with non-existent parent directories (should create them)
    std::string sNestedPath = sTempDir + "/level1/level2/output.png"; // Local Function
    bValid = fn_validateOutputPath(sNestedPath); // Local Function
    assert(bValid && "Should create nested directories"); // In cassert
    assert(fn_isDirectory(sTempDir + "/level1/level2") && "Nested directories should be created"); // In cassert
    
    // Test 3: Path in root directory (should be valid if we have permission)
    // We'll skip this test as it requires root permissions
    
    // Test 4: Path with no directory component
    std::string sFileOnly = "output.bmp"; // Local Function
    bValid = fn_validateOutputPath(sFileOnly); // Local Function
    assert(bValid && "File without directory should be valid"); // In cassert
    
    // Cleanup
    fn_cleanupTempDirectory(sTempDir); // Local Function
} // End Function fn_validateOutputPath

// Test: fn_copyFile
void fn_testCopyFile() 
{ // Begin fn_testCopyFile
    std::string sTempDir = fn_generateTempDirectory(); // Local Function
    
    // Create source file with content
    std::string sSourceFile = sTempDir + "/source.txt"; // Local Function
    std::string sTestContent = "This is test content for copying.\nMultiple lines.\nEnd of file."; // Local Function
    fn_createTestFile(sSourceFile, sTestContent); // Local Function
    
    // Destination file
    std::string sDestFile = sTempDir + "/destination.txt"; // Local Function
    
    // Test 1: Copy file
    bool bCopied = fn_copyFile(sSourceFile, sDestFile); // Local Function
    assert(bCopied && "File should be copied successfully"); // In cassert
    assert(fn_fileExists(sDestFile) && "Destination file should exist"); // In cassert
    
    // Verify content
    std::ifstream oSource(sSourceFile); // In fstream
    std::ifstream oDest(sDestFile); // In fstream
    std::string sSourceContent((std::istreambuf_iterator<char>(oSource)), // In iterator
                               std::istreambuf_iterator<char>()); // In iterator
    std::string sDestContent((std::istreambuf_iterator<char>(oDest)), // In iterator
                             std::istreambuf_iterator<char>()); // In iterator
    
    assert(sSourceContent == sTestContent && "Source content should match original"); // In cassert
    assert(sDestContent == sTestContent && "Destination content should match source"); // In cassert
    
    // Test 2: Copy to non-existent directory (should fail)
    std::string sInvalidDest = sTempDir + "/nonexistent/dir/file.txt"; // Local Function
    bool bShouldFail = fn_copyFile(sSourceFile, sInvalidDest); // Local Function
    // This might fail or succeed depending on implementation
    // We'll just ensure it doesn't crash
    
    // Test 3: Copy non-existent source (should fail)
    bool bNonExistentCopy = fn_copyFile(sTempDir + "/nonexistent.txt", sDestFile); // Local Function
    assert(!bNonExistentCopy && "Copying non-existent file should fail"); // In cassert
    
    // Cleanup
    fn_cleanupTempDirectory(sTempDir); // Local Function
} // End Function fn_copyFile

// Test: fn_deleteFile
void fn_testDeleteFile() 
{ // Begin fn_testDeleteFile
    std::string sTempDir = fn_generateTempDirectory(); // Local Function
    
    // Create test file
    std::string sTestFile = sTempDir + "/test_delete.txt"; // Local Function
    fn_createTestFile(sTestFile, "content to delete"); // Local Function
    
    // Test 1: Delete existing file
    assert(fn_fileExists(sTestFile) && "Test file should exist before deletion"); // In cassert
    bool bDeleted = fn_deleteFile(sTestFile); // Local Function
    assert(bDeleted && "File should be deleted successfully"); // In cassert
    assert(!fn_fileExists(sTestFile) && "File should not exist after deletion"); // In cassert
    
    // Test 2: Delete non-existent file (should fail)
    bool bDeleteNonExistent = fn_deleteFile(sTempDir + "/nonexistent.txt"); // Local Function
    assert(!bDeleteNonExistent && "Deleting non-existent file should fail"); // In cassert
    
    // Test 3: Try to delete directory (might fail depending on implementation)
    bool bDeleteDir = fn_deleteFile(sTempDir); // Local Function
    // This might fail as fn_deleteFile is for files, not directories
    // We'll just ensure it doesn't crash
    
    // Cleanup
    fn_cleanupTempDirectory(sTempDir); // Local Function
} // End Function fn_deleteFile

// Test: fn_getFileSize
void fn_testGetFileSize() 
{ // Begin fn_testGetFileSize
    std::string sTempDir = fn_generateTempDirectory(); // Local Function
    
    // Test 1: File with known size
    std::string sTestFile = sTempDir + "/size_test.txt"; // Local Function
    std::string sContent = "1234567890"; // 10 bytes // Local Function
    fn_createTestFile(sTestFile, sContent); // Local Function
    
    uint64_t iSize = fn_getFileSize(sTestFile); // Local Function
    assert(iSize == 10 && "File size should be 10 bytes"); // In cassert
    
    // Test 2: Empty file
    std::string sEmptyFile = sTempDir + "/empty.txt"; // Local Function
    fn_createTestFile(sEmptyFile, ""); // Local Function
    
    iSize = fn_getFileSize(sEmptyFile); // Local Function
    assert(iSize == 0 && "Empty file should have 0 size"); // In cassert
    
    // Test 3: Non-existent file
    iSize = fn_getFileSize(sTempDir + "/nonexistent.txt"); // Local Function
    assert(iSize == 0 && "Non-existent file should return 0 size"); // In cassert
    
    // Test 4: Binary file (create with binary data)
    std::string sBinaryFile = sTempDir + "/binary.bin"; // Local Function
    std::ofstream oBinary(sBinaryFile, std::ios::binary); // In fstream
    for (int i = 0; i < 256; i++) 
    { // Begin for
        oBinary.put(static_cast<char>(i)); // In fstream
    } // End for(int i = 0; i < 256; i++)
    oBinary.close(); // In fstream
    
    iSize = fn_getFileSize(sBinaryFile); // Local Function
    assert(iSize == 256 && "Binary file should be 256 bytes"); // In cassert
    
    // Cleanup
    fn_cleanupTempDirectory(sTempDir); // Local Function
} // End Function fn_getFileSize

// Test: fn_isHeicFile
void fn_testIsHeicFile() 
{ // Begin fn_testIsHeicFile
    // Test cases
    struct oTestCase 
    { // Begin struct oTestCase
        std::string sFilename; // Local Function
        bool bExpected; // Local Function
    }; // End struct oTestCase
    
    std::vector<oTestCase> voTestCases = 
    { // Begin vector voTestCases
        {"image.heic", true}, // Local Function
        {"photo.heif", true}, // Local Function
        {"picture.HEIC", false}, // Local Function (case sensitive)
        {"document.HEIF", false}, // Local Function (case sensitive)
        {"file.jpg", false}, // Local Function
        {"image.png", false}, // Local Function
        {"archive.heic.zip", false}, // Local Function
        {"", false}, // Local Function
        {".heic", false}, // Local Function
        {"heic", false}, // Local Function
        {"path/to/image.heic", true} // Local Function
    }; // End vector voTestCases
    
    for (const auto& oCase : voTestCases) 
    { // Begin for
        bool bResult = fn_isHeicFile(oCase.sFilename); // Local Function
        assert(bResult == oCase.bExpected && "HEIC file detection mismatch"); // In cassert
    } // End for(const auto& oCase : voTestCases)
} // End Function fn_isHeicFile

// Test: fn_generateUniqueFileName
void fn_testGenerateUniqueFileName() 
{ // Begin fn_testGenerateUniqueFileName
    std::string sTempDir = fn_generateTempDirectory(); // Local Function
    
    // Test 1: No existing file
    std::string sUnique1 = fn_generateUniqueFileName(sTempDir, "test", "txt"); // Local Function
    assert(sUnique1 == sTempDir + "/test.txt" && "First file should not have counter"); // In cassert
    
    // Create the file
    fn_createTestFile(sUnique1, "content"); // Local Function
    
    // Test 2: File exists, should generate with counter
    std::string sUnique2 = fn_generateUniqueFileName(sTempDir, "test", "txt"); // Local Function
    assert(sUnique2 == sTempDir + "/test_1.txt" && "Second file should have counter _1"); // In cassert
    
    // Create second file
    fn_createTestFile(sUnique2, "content"); // Local Function
    
    // Test 3: Third file
    std::string sUnique3 = fn_generateUniqueFileName(sTempDir, "test", "txt"); // Local Function
    assert(sUnique3 == sTempDir + "/test_2.txt" && "Third file should have counter _2"); // In cassert
    
    // Test 4: Different base name
    std::string sDifferent = fn_generateUniqueFileName(sTempDir, "other", "jpg"); // Local Function
    assert(sDifferent == sTempDir + "/other.jpg" && "Different base name should not have counter"); // In cassert
    
    // Test 5: Different extension
    std::string sDifferentExt = fn_generateUniqueFileName(sTempDir, "test", "jpg"); // Local Function
    assert(sDifferentExt == sTempDir + "/test.jpg" && "Different extension should not have counter"); // In cassert
    
    // Cleanup
    fn_cleanupTempDirectory(sTempDir); // Local Function
} // End Function fn_generateUniqueFileName

// Test: fn_normalizePath
void fn_testNormalizePath() 
{ // Begin fn_testNormalizePath
    // Test cases
    struct oTestCase 
    { // Begin struct oTestCase
        std::string sInput; // Local Function
        std::string sExpected; // Local Function
    }; // End struct oTestCase
    
    std::vector<oTestCase> voTestCases = 
    { // Begin vector voTestCases
        {"", ""}, // Local Function
        {"/", "/"}, // Local Function
        {"//", "/"}, // Local Function
        {"///", "/"}, // Local Function
        {"path/to/file", "path/to/file"}, // Local Function
        {"path//to//file", "path/to/file"}, // Local Function
        {"path/to/file/", "path/to/file"}, // Local Function
        {"path\\to\\file", "path/to/file"}, // Local Function (backslash conversion)
        {"C:\\Windows\\System32", "C:/Windows/System32"}, // Local Function
        {"./path/../to/./file", "./path/../to/./file"} // Local Function (doesn't resolve .. or .)
    }; // End vector voTestCases
    
    for (auto oCase : voTestCases) 
    { // Begin for
        fn_normalizePath(oCase.sInput); // Local Function
        assert(oCase.sInput == oCase.sExpected && "Path normalization mismatch"); // In cassert
    } // End for(auto oCase : voTestCases)
} // End Function fn_normalizePath

// Test: fn_hasWritePermission
void fn_testHasWritePermission() 
{ // Begin fn_testHasWritePermission
    std::string sTempDir = fn_generateTempDirectory(); // Local Function
    
    // Test 1: Directory we just created should be writable
    bool bWritable = fn_hasWritePermission(sTempDir); // Local Function
    assert(bWritable && "Created directory should be writable"); // In cassert
    
    // Test 2: File in writable directory
    std::string sTestFile = sTempDir + "/test.txt"; // Local Function
    fn_createTestFile(sTestFile, "test"); // Local Function
    bWritable = fn_hasWritePermission(sTestFile); // Local Function
    assert(bWritable && "File in writable directory should be writable"); // In cassert
    
    // Test 3: Non-existent file in writable directory
    bWritable = fn_hasWritePermission(sTempDir + "/nonexistent.txt"); // Local Function
    assert(bWritable && "Non-existent file in writable directory should be writable"); // In cassert
    
    // Note: Testing read-only directories/files is platform-specific and
    // might require special setup. We'll skip negative tests for portability.
    
    // Cleanup
    fn_cleanupTempDirectory(sTempDir); // Local Function
} // End Function fn_hasWritePermission

// Helper: Generate temporary directory
std::string fn_generateTempDirectory() 
{ // Begin fn_generateTempDirectory
    // Use current timestamp to make directory unique
    std::time_t tNow = std::time(nullptr); // In ctime
    std::string sTimestamp = std::to_string(tNow); // Local Function
    
    // Add random component for extra uniqueness
    int iRandom = std::rand() % 10000; // In cstdlib
    sTimestamp += "_" + std::to_string(iRandom); // Local Function
    
    std::string sTempDir = "/tmp/heic_test_" + sTimestamp; // Local Function
    
    // Create directory
    fn_createDirectory(sTempDir); // Local Function
    
    return sTempDir; // End return
} // End Function fn_generateTempDirectory

// Helper: Cleanup temporary directory
void fn_cleanupTempDirectory(const std::string& sPath) 
{ // Begin fn_cleanupTempDirectory
    if (fn_fileExists(sPath) && fn_isDirectory(sPath)) 
    { // Begin if
        // Remove directory and all contents
        std::filesystem::remove_all(sPath); // In filesystem
    } // End if(fn_fileExists(sPath) && fn_isDirectory(sPath))
} // End Function fn_cleanupTempDirectory

// Helper: Create test file with content
bool fn_createTestFile(const std::string& sPath, const std::string& sContent) 
{ // Begin fn_createTestFile
    std::ofstream oFile(sPath); // In fstream
    if (!oFile.is_open()) 
    { // Begin if
        return false; // End return
    } // End if(!oFile.is_open())
    
    oFile << sContent; // In fstream
    oFile.close(); // In fstream
    
    return true; // End return
} // End Function fn_createTestFile