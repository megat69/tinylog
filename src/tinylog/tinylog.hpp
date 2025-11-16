#pragma once
/**
 * @file TinyLog, a stupidly simple C++ logging framework.
 */
#include <vector>
#include <ostream>
#include <cassert>

/// @brief Current version of TinyLog. Follows [Semantic Versioning](https://semver.org/).
#define TINYLOG_VERSION "0.0.1"

/// @brief If set to 1, makes sure TinyLog uses its own namespace for functions and classes ; but not macros. Is 1 by default.
#ifndef TINYLOG_USE_NAMESPACE
#define TINYLOG_USE_NAMESPACE 1
#endif

/// @brief Sets the default LogLevel for TinyLog in debug mode
#ifndef TINYLOG_DEFAULT_DEBUG_MODE_LOG_LEVEL
#define TINYLOG_DEFAULT_DEBUG_MODE_LOG_LEVEL INFO
#endif

/// @brief Sets the default LogLevel for TinyLog in release mode
#ifndef TINYLOG_DEFAULT_RELEASE_MODE_LOG_LEVEL
#define TINYLOG_DEFAULT_RELEASE_MODE_LOG_LEVEL WARN
#endif

/// @brief Sets the default LogLevel for TinyLog based on the debug/release configuration
#ifndef TINYLOG_DEFAULT_LOG_LEVEL
#ifndef NDEBUG  // Debug mode
#define TINYLOG_DEFAULT_LOG_LEVEL TINYLOG_DEFAULT_DEBUG_MODE_LOG_LEVEL
#else
#define TINYLOG_DEFAULT_LOG_LEVEL TINYLOG_DEFAULT_RELEASE_MODE_LOG_LEVEL
#endif
#endif

#if TINYLOG_USE_NAMESPACE == 1
namespace TinyLog {
#endif

enum LogLevel: char {
    DEBUG = 0,
    INFO,
    WARN,
    ERROR,
    FATAL,

    INHERIT = -1
};

/**
 * @brief Returns the stringified version of the given log level
 * @param logLevel A log level
 * @param doPad If true, adds spaces at the end of the returned string until the string is 5 characters long
 * @returns The name of the log level, as a string
 */
std::string getLogLevelName(LogLevel logLevel, bool doPad = false) {
    std::string returnString = "";
    switch (logLevel) {
        case DEBUG:   returnString = "DEBUG"; break;
        case INFO:    returnString = "INFO"; break;
        case WARN:    returnString = "WARN"; break;
        case ERROR:   returnString = "ERROR"; break;
        case FATAL:   returnString = "FATAL"; break;
        case INHERIT: returnString = "INHERIT"; break;
        default: std::terminate();
    }
    if (doPad) {
        while (returnString.size() < 5)
            returnString += " ";
    }
    return returnString;
}

class Logger  {
private:
    /// @brief A list of loggers as a hierarchy
    inline static std::vector<Logger*> loggers{};

    /// @brief A pointer to the output stream for the string logging
    inline static std::ostream* stringOutputStream = nullptr;

    /// @brief Whether string logging is enabled
    inline static bool isStringOutputEnabled = false;

    /// @brief The log level for this logger
    LogLevel currentLogLevel;

public:
    explicit Logger(LogLevel logLevel = INHERIT) {
        loggers.push_back(this);
        currentLogLevel = logLevel;
    }

    /**
     * @brief Returns the log level for this logger, going back up the chain of loggers until one isn't set to INHERIT.
     * @note If every logger in the chain's log level is set to `INHERIT` (including the top-most logger's), then the value
     *      of the macro `TINYLOG_DEFAULT_LOG_LEVEL` is returned.
     * @returns A log level.
     */
    LogLevel getLogLevel() const {
        int currentLoggerIndex = loggers.size() - 1;
        assert(currentLoggerIndex >= 0);
        while (loggers[currentLoggerIndex]->currentLogLevel == INHERIT) {
            if (currentLoggerIndex == 0)
                return TINYLOG_DEFAULT_LOG_LEVEL;
            currentLoggerIndex -= 1;
        }
        return loggers[currentLoggerIndex]->currentLogLevel;
    }

    /**
     * @brief Logs the given message if the given log level is above the current log level
     * @param givenLogLevel The log level for this log
     * @param message The message to log
     */
    void log(LogLevel givenLogLevel, const std::string& message) {
        if (static_cast<char>(givenLogLevel) < static_cast<char>(getLogLevel())) return;
        if (isStringOutputEnabled) {
            assert(stringOutputStream != nullptr);
            *stringOutputStream << "[" << getLogLevelName(givenLogLevel, true) << "] " << message << "\n";
        }
    }

    /**
     * @brief Enables logging to a given stream, as a string output.
     * @warning Replaces the previous output stream with no further processing.
     * @param outputStream An output stream for the logging. Example : std::cout
     */
    static void enableStringOutput(std::ostream& outputStream) {
        stringOutputStream = &outputStream;
        isStringOutputEnabled = true;
    }

    /**
     * @brief Disables logging as a string output.
     * @warning Removes the previous output stream with no further processing.
     */
    static void disableStringOutput() {
        stringOutputStream = nullptr;
        isStringOutputEnabled = false;
    }

    /**
     * @brief Returns whether the string output is enabled
     * @note This function should be marked as const, but is a static function.
     * @returns Whether the string output is enabled.
     */
    static bool isEnabledStringOutput() {
        return isStringOutputEnabled;
    }
};

#if TINYLOG_USE_NAMESPACE == 1
}  // TinyLog
#endif
