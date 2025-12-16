// logger.cpp - Logging implementation for HEIC/HEIF Converter
// Author: R Square Innovation Software
// Version: v1.0

#include "logger.h"
#include <iomanip>
#include <sstream>

// Global logger instance
oLogger g_oLogger;  // Local Function

// Constructor
oLogger::oLogger() {  // Begin Constructor
    bVerboseMode = false;  // End bVerboseMode
    bDebugMode = false;  // End bDebugMode
    bLogToFile = false;  // End bLogToFile
    eMinimumLevel = LOG_INFO;  // Default to INFO level
    sLogFilename = "";  // End sLogFilename
}  // End Constructor

// Destructor
oLogger::~oLogger() {  // Begin Destructor
    if (oLogFile.is_open()) {  // Begin if
        oLogFile.close();  // End oLogFile.close()
    }  // End if(oLogFile.is_open())
}  // End Destructor

// Set verbose mode
void oLogger::fn_setVerbose(bool bVerbose) {  // Begin fn_setVerbose
    bVerboseMode = bVerbose;  // End bVerboseMode
    if (bVerbose) {  // Begin if
        eMinimumLevel = LOG_INFO;  // Show INFO and above
    }  // End if(bVerbose)
    else {  // Begin else
        eMinimumLevel = LOG_WARNING;  // Show WARNING and above only
    }  // End else
}  // End Function fn_setVerbose

// Set debug mode
void oLogger::fn_setDebug(bool bDebug) {  // Begin fn_setDebug
    bDebugMode = bDebug;  // End bDebugMode
    if (bDebug) {  // Begin if
        eMinimumLevel = LOG_DEBUG;  // Show DEBUG and above
    }  // End if(bDebug)
}  // End Function fn_setDebug

// Set log file
void oLogger::fn_setLogFile(const std::string& sFilename) {  // Begin fn_setLogFile
    std::lock_guard<std::mutex> oLock(oLogMutex);  // In mutex
    
    // Close existing log file
    if (oLogFile.is_open()) {  // Begin if
        oLogFile.close();  // End oLogFile.close()
    }  // End if(oLogFile.is_open())
    
    // Open new log file
    sLogFilename = sFilename;  // End sLogFilename
    oLogFile.open(sFilename, std::ios::out | std::ios::app);  // End oLogFile.open()
    
    if (oLogFile.is_open()) {  // Begin if
        bLogToFile = true;  // End bLogToFile
        fn_logInfo("Logging to file: " + sFilename);  // Local Function
    }  // End if(oLogFile.is_open())
    else {  // Begin else
        bLogToFile = false;  // End bLogToFile
        fn_logError("Failed to open log file: " + sFilename);  // Local Function
    }  // End else
}  // End Function fn_setLogFile

// Set minimum log level
void oLogger::fn_setMinimumLevel(eLogLevel eLevel) {  // Begin fn_setMinimumLevel
    eMinimumLevel = eLevel;  // End eMinimumLevel
}  // End Function fn_setMinimumLevel

// Log error message
void oLogger::fn_logError(const std::string& sMessage) {  // Begin fn_logError
    fn_internalLog(LOG_ERROR, sMessage);  // Local Function
}  // End Function fn_logError

// Log warning message
void oLogger::fn_logWarning(const std::string& sMessage) {  // Begin fn_logWarning
    fn_internalLog(LOG_WARNING, sMessage);  // Local Function
}  // End Function fn_logWarning

// Log info message
void oLogger::fn_logInfo(const std::string& sMessage) {  // Begin fn_logInfo
    fn_internalLog(LOG_INFO, sMessage);  // Local Function
}  // End Function fn_logInfo

// Log debug message
void oLogger::fn_logDebug(const std::string& sMessage) {  // Begin fn_logDebug
    if (bDebugMode) {  // Begin if
        fn_internalLog(LOG_DEBUG, sMessage);  // Local Function
    }  // End if(bDebugMode)
}  // End Function fn_logDebug

// Log success message
void oLogger::fn_logSuccess(const std::string& sMessage) {  // Begin fn_logSuccess
    fn_internalLog(LOG_SUCCESS, sMessage);  // Local Function
}  // End Function fn_logSuccess

// Check if verbose mode is enabled
bool oLogger::fn_isVerbose() const {  // Begin fn_isVerbose
    return bVerboseMode;  // End return bVerboseMode
}  // End Function fn_isVerbose

// Check if debug mode is enabled
bool oLogger::fn_isDebug() const {  // Begin fn_isDebug
    return bDebugMode;  // End return bDebugMode
}  // End Function fn_isDebug

// Get string representation of log level
std::string oLogger::fn_getLogLevelString(eLogLevel eLevel) const {  // Begin fn_getLogLevelString
    switch (eLevel) {  // Begin switch
        case LOG_ERROR: return "ERROR";  // End case LOG_ERROR
        case LOG_WARNING: return "WARNING";  // End case LOG_WARNING
        case LOG_INFO: return "INFO";  // End case LOG_INFO
        case LOG_DEBUG: return "DEBUG";  // End case LOG_DEBUG
        case LOG_SUCCESS: return "SUCCESS";  // End case LOG_SUCCESS
        default: return "UNKNOWN";  // End default
    }  // End switch(eLevel)
}  // End Function fn_getLogLevelString

// Flush log buffers
void oLogger::fn_flush() {  // Begin fn_flush
    std::lock_guard<std::mutex> oLock(oLogMutex);  // In mutex
    
    if (oLogFile.is_open()) {  // Begin if
        oLogFile.flush();  // End oLogFile.flush()
    }  // End if(oLogFile.is_open())
    
    std::cout.flush();  // In iostream
    std::cerr.flush();  // In iostream
}  // End Function fn_flush

// Internal logging function
void oLogger::fn_internalLog(eLogLevel eLevel, const std::string& sMessage) {  // Begin fn_internalLog
    // Check if message should be logged based on minimum level
    if (eLevel > eMinimumLevel && eLevel != LOG_SUCCESS) {  // Begin if
        return;  // Skip this message
    }  // End if(eLevel > eMinimumLevel && eLevel != LOG_SUCCESS)
    
    std::lock_guard<std::mutex> oLock(oLogMutex);  // In mutex
    
    // Format the message
    std::string sFormattedMessage = fn_formatMessage(eLevel, sMessage);  // Local Function
    
    // Output to appropriate stream
    if (eLevel == LOG_ERROR) {  // Begin if
        std::cerr << sFormattedMessage << std::endl;  // In iostream
    }  // End if(eLevel == LOG_ERROR)
    else {  // Begin else
        std::cout << sFormattedMessage << std::endl;  // In iostream
    }  // End else
    
    // Write to log file if enabled
    if (bLogToFile && oLogFile.is_open()) {  // Begin if
        oLogFile << sFormattedMessage << std::endl;  // In fstream
        oLogFile.flush();  // In fstream
    }  // End if(bLogToFile && oLogFile.is_open())
}  // End Function fn_internalLog

// Get current time as string
std::string oLogger::fn_getCurrentTime() const {  // Begin fn_getCurrentTime
    std::time_t tNow = std::time(nullptr);  // In ctime
    std::tm* pTimeInfo = std::localtime(&tNow);  // In ctime
    
    std::ostringstream oss;  // In sstream
    oss << std::put_time(pTimeInfo, "%Y-%m-%d %H:%M:%S");  // In iomanip
    
    return oss.str();  // End return oss.str()
}  // End Function fn_getCurrentTime

// Format message with timestamp and log level
std::string oLogger::fn_formatMessage(eLogLevel eLevel, const std::string& sMessage) const {  // Begin fn_formatMessage
    std::string sLevelString = fn_getLogLevelString(eLevel);  // Local Function
    std::string sTimestamp = fn_getCurrentTime();  // Local Function
    
    std::ostringstream oss;  // In sstream
    
    // Add color codes for terminal output (optional)
    bool bUseColors = true;  // Can be made configurable
    
    if (bUseColors) {  // Begin if
        switch (eLevel) {  // Begin switch
            case LOG_ERROR: oss << "\033[1;31m"; break;  // Red
            case LOG_WARNING: oss << "\033[1;33m"; break;  // Yellow
            case LOG_INFO: oss << "\033[1;36m"; break;  // Cyan
            case LOG_DEBUG: oss << "\033[1;35m"; break;  // Magenta
            case LOG_SUCCESS: oss << "\033[1;32m"; break;  // Green
            default: oss << "\033[0m"; break;  // Reset
        }  // End switch(eLevel)
    }  // End if(bUseColors)
    
    oss << "[" << sTimestamp << "] [" << sLevelString << "] " << sMessage;  // In sstream
    
    if (bUseColors) {  // Begin if
        oss << "\033[0m";  // Reset color
    }  // End if(bUseColors)
    
    return oss.str();  // End return oss.str()
}  // End Function fn_formatMessage

// Convenience functions (using global logger instance)

// Log error message (global)
void fn_logError(const std::string& sMessage) {  // Begin fn_logError
    g_oLogger.fn_logError(sMessage);  // Local Function
}  // End Function fn_logError

// Log warning message (global)
void fn_logWarning(const std::string& sMessage) {  // Begin fn_logWarning
    g_oLogger.fn_logWarning(sMessage);  // Local Function
}  // End Function fn_logWarning

// Log info message (global)
void fn_logInfo(const std::string& sMessage) {  // Begin fn_logInfo
    g_oLogger.fn_logInfo(sMessage);  // Local Function
}  // End Function fn_logInfo

// Log debug message (global)
void fn_logDebug(const std::string& sMessage) {  // Begin fn_logDebug
    g_oLogger.fn_logDebug(sMessage);  // Local Function
}  // End Function fn_logDebug

// Log success message (global)
void fn_logSuccess(const std::string& sMessage) {  // Begin fn_logSuccess
    g_oLogger.fn_logSuccess(sMessage);  // Local Function
}  // End Function fn_logSuccess