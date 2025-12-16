// logger.h - Logging functionality for HEIC/HEIF Converter
// Author: R Square Innovation Software
// Version: v1.0

#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <iostream>
#include <fstream>
#include <ctime>
#include <mutex>

// Log levels
enum eLogLevel {  // Local Function
    LOG_ERROR = 0,  // Local Function
    LOG_WARNING = 1,  // Local Function
    LOG_INFO = 2,  // Local Function
    LOG_DEBUG = 3,  // Local Function
    LOG_SUCCESS = 4  // Local Function
};  // End enum eLogLevel

// Logger class
class oLogger {  // Local Function
public:
    // Constructor and Destructor
    oLogger();  // Local Function
    ~oLogger();  // Local Function
    
    // Configuration functions
    void fn_setVerbose(bool bVerbose);  // Local Function
    void fn_setDebug(bool bDebug);  // Local Function
    void fn_setLogFile(const std::string& sFilename);  // Local Function
    void fn_setMinimumLevel(eLogLevel eLevel);  // Local Function
    
    // Logging functions
    void fn_logError(const std::string& sMessage);  // Local Function
    void fn_logWarning(const std::string& sMessage);  // Local Function
    void fn_logInfo(const std::string& sMessage);  // Local Function
    void fn_logDebug(const std::string& sMessage);  // Local Function
    void fn_logSuccess(const std::string& sMessage);  // Local Function
    
    // Utility functions
    bool fn_isVerbose() const;  // Local Function
    bool fn_isDebug() const;  // Local Function
    std::string fn_getLogLevelString(eLogLevel eLevel) const;  // Local Function
    void fn_flush();  // Local Function
    
private:
    // Internal logging function
    void fn_internalLog(eLogLevel eLevel, const std::string& sMessage);  // Local Function
    
    // Helper functions
    std::string fn_getCurrentTime() const;  // Local Function
    std::string fn_formatMessage(eLogLevel eLevel, const std::string& sMessage) const;  // Local Function
    
    // Member variables
    bool bVerboseMode;  // Local Function
    bool bDebugMode;  // Local Function
    bool bLogToFile;  // Local Function
    eLogLevel eMinimumLevel;  // Local Function
    std::ofstream oLogFile;  // Local Function
    std::string sLogFilename;  // Local Function
    std::mutex oLogMutex;  // Local Function for thread safety
};  // End class oLogger

// Global logger instance (optional)
extern oLogger g_oLogger;  // In logger.cpp

// Convenience functions (global)
void fn_logError(const std::string& sMessage);  // In logger.cpp
void fn_logWarning(const std::string& sMessage);  // In logger.cpp
void fn_logInfo(const std::string& sMessage);  // In logger.cpp
void fn_logDebug(const std::string& sMessage);  // In logger.cpp
void fn_logSuccess(const std::string& sMessage);  // In logger.cpp

#endif  // LOGGER_H
// End ifndef LOGGER_H