#pragma once
/**
 * @file TinyLog, a stupidly simple C++ logging framework.
 */
#include <vector>
#include <ostream>
#include <cassert>
#include <ctime>
#include <string>
#include <algorithm>

/// @brief Current version of TinyLog. Follows [Semantic Versioning](https://semver.org/).
#define TINYLOG_VERSION "0.6.0"

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

/// @brief If set to 1, log extras will be put on their own separate lines
#ifndef TINYLOG_EXTRAS_ON_SEPARATE_LINES
#define TINYLOG_EXTRAS_ON_SEPARATE_LINES 0
#endif

#ifdef SOURCE_PATH_SIZE
#define __FILENAME__ (__FILE__ + SOURCE_PATH_SIZE)
#else
#define __FILENAME__ __FILE__
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

    /// @brief A vector of pointers to the output stream for the string logging
    inline static std::vector<std::ostream*> stringOutputStreams = {};

    /// @brief Whether string logging is enabled
    inline static bool isStringOutputEnabled = false;

    /// @brief A vector of pointer to the output streams for JSON logging
    inline static std::vector<std::ostream*> jsonOutputStreams = {};

    /// @brief Whether JSON logging is enabled
    inline static bool isJsonOutputEnabled = false;

    /// @brief How many logs have been sent to JSON
    inline static long long jsonOutputsCount = 0;

    /// @brief The log level for this logger
    LogLevel currentLogLevel;

public:
    explicit Logger(LogLevel logLevel = INHERIT) {
        loggers.push_back(this);
        currentLogLevel = logLevel;
    }

    ~Logger() {
        disableStringOutput();
        disableJsonOutput();
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
     * @param extras An ensemble of strings to be displayed after the message
     * @param filePath The path to the file where this function is called. Not displayed by default.
     * @param lineNumber The line number of the file where this function is called. Not displayed by default.
     */
    void log(LogLevel givenLogLevel, const std::string& message, std::initializer_list<std::string> extras = {}, std::string filePath = "", int lineNumber = -1, bool showTimestamp = true) {
        // Shortcut to exit the function if the log level does not match
        if (static_cast<char>(givenLogLevel) < static_cast<char>(getLogLevel())) return;

        // String output
        if (isStringOutputEnabled) {
            for (std::ostream* stringOutputStream : stringOutputStreams) {
                logToStringOutputStream(stringOutputStream, givenLogLevel, showTimestamp, filePath, lineNumber, message, extras);
            }
        }

        // JSON output
        if (isJsonOutputEnabled) {
            // Copies strings and escapes quotes inside them
            std::string messageCopy = message;
            std::vector<std::string> extrasCopy = extras;
            std::replace(messageCopy.begin(), messageCopy.end(), '\"', '\'');
            for (std::string& extra : extrasCopy) {
                std::replace(extra.begin(), extra.end(), '\"', '\'');
            }

            for (std::ostream* jsonOutputStream : jsonOutputStreams) {
                logToJsonOutputStream(jsonOutputStream, givenLogLevel, showTimestamp, filePath, lineNumber, messageCopy, extrasCopy);
            }

            // Remembers how many JSON logs have been outputted
            jsonOutputsCount++;
        }
    }

    /**
     * @brief Enables logging to a given stream, as a string output.
     * @param outputStream An output stream for the logging. Example : std::cout
     */
    static void enableStringOutput(std::ostream& outputStream) {
        isStringOutputEnabled = true;
        addStringOutput(outputStream);
    }

    /**
     * @bref Adds another output stream to the string output.
     * @param outputStream An output stream for the logging. Example : std::cout
     */
    static void addStringOutput(std::ostream& outputStream) {
        assert(isStringOutputEnabled);
        stringOutputStreams.push_back(&outputStream);
    }

    /**
     * @brief Disables logging as a string output.
     * @warning Clears the previous output streams with no further processing.
     */
    static void disableStringOutput() {
        stringOutputStreams.clear();
        isStringOutputEnabled = false;
    }

    /**
     * @brief Enables logging to a given stream, as a JSON output.
     * @param outputStream An output stream for the logging. Example : std::cout
     */
    static void enableJsonOutput(std::ostream& outputStream) {
        isJsonOutputEnabled = true;
        jsonOutputsCount = 0;
        addJsonOutput(outputStream);
    }

    /**
     * @bref Adds another output stream to the JSON output.
     * @param outputStream An output stream for the logging. Example : std::cout
     */
    static void addJsonOutput(std::ostream& outputStream) {
        assert(isJsonOutputEnabled);
        jsonOutputStreams.push_back(&outputStream);
        outputStream << "[";
    }

    /**
     * @brief Disables logging as a JSON output.
     * @warning Clears the previous output streams with minor processing.
     */
    static void disableJsonOutput() {
        for (std::ostream* outputStream : jsonOutputStreams) {
            *outputStream << "]";
        }
        jsonOutputStreams.clear();
        isJsonOutputEnabled = false;
        jsonOutputsCount = 0;
    }

    /**
     * @brief Returns whether the string output is enabled
     * @note This function should be marked as const, but is a static function.
     * @returns Whether the string output is enabled.
     */
    static bool isEnabledStringOutput() {
        return isStringOutputEnabled;
    }

    /**
     * @brief Pre-allocates enough space for n loggers
     * @param capacity The new logger chain capacity
     * @warning `capacity` must be higher than the current amount of loggers in the chain. An assertion will check that.
     */
    static void setLoggerChainCapacity(size_t capacity) {
        assert(capacity > loggers.size());  // Avoids UB
        loggers.reserve(capacity);
    }

private:
    /**
     * @brief Calculates the ISO 8601 timestamp, and returns it.
     * @returns The current ISO 8601 timestamp.
     */
    std::string getIso8601Timestamp() {
        std::time_t now;
        std::time(&now);
        char timestampBuffer[sizeof "1970-01-01T00:00:00Z"];
        std::strftime(timestampBuffer, sizeof timestampBuffer, "%FT%TZ", std::gmtime(&now));
        return timestampBuffer;
    }

    /**
     * @brief Escapes the given string's double quotes
     * @param stringToEscape A string with possibly double quotes inside, to be escaped.
     * @warning This operation is in place.
     */
    void escapeString(std::string& stringToEscape) {
        std::replace(stringToEscape.begin(), stringToEscape.end(), '\"', '\'');
    }

    /**
     * @brief Logs to the given outputs stream, as a string
     */
    void logToStringOutputStream(std::ostream *stringOutputStream, LogLevel givenLogLevel, bool showTimestamp, const std::string& filePath, int lineNumber, const std::string& message, std::initializer_list<std::string>& extras) {
        assert(stringOutputStream != nullptr);
        std::string logLevelName = getLogLevelName(givenLogLevel, true);
        *stringOutputStream << "[" << logLevelName << "] ";
        if (showTimestamp) {
            std::string timestamp = getIso8601Timestamp();
            *stringOutputStream << timestamp << " - ";
        }
        if (filePath != "") {
            *stringOutputStream << filePath << " ";
        }
        if (lineNumber != -1) {
            *stringOutputStream << "(line " << lineNumber << ") ";
        }
        if (filePath != "" || lineNumber != -1) {
            *stringOutputStream << "- ";
        }
        *stringOutputStream << message;
        if (extras.size() > 0) {
            *stringOutputStream << " - EXTRAS " << ((TINYLOG_EXTRAS_ON_SEPARATE_LINES) ? ":" : "- ");
        }
        for (const std::string &extra : extras) {
            *stringOutputStream << ((TINYLOG_EXTRAS_ON_SEPARATE_LINES) ? ("\n" + std::string(logLevelName.size() + 3, ' ') + "- ") : " ") << extra << " ;";
        }
        *stringOutputStream << "\n";
    }

    /**
     * @brief Logs to the given outputs stream, as JSON
     */
    void logToJsonOutputStream(std::ostream *jsonOutputStream, LogLevel givenLogLevel, bool showTimestamp, std::string& filePath, int lineNumber, const std::string& message, std::vector<std::string>& extras) {
        if (jsonOutputsCount > 0) {
            *jsonOutputStream << ",";
        }
        *jsonOutputStream << "{\"severity\":\"" << getLogLevelName(givenLogLevel) << "\",";
        *jsonOutputStream << "\"message\":\"" << message << "\",";
        *jsonOutputStream << "\"timestamp\":\"" << getIso8601Timestamp() << "\"";
        if (extras.size() > 0) {
            *jsonOutputStream << ",\"extras\":[";
            for (int i = 0; i < extras.size(); i++) {
                *jsonOutputStream << "\"" << extras.at(i) << "\"";
                if (i < extras.size() - 1) {
                    *jsonOutputStream << ",";
                }
            }
            *jsonOutputStream << "]";
        }
        *jsonOutputStream << "}";
    }
};

#if TINYLOG_USE_NAMESPACE == 1
}  // TinyLog
#endif

/**
 * @brief Returns a string containing the given expression and its value
 * @param expression A C++ expression that can be converted to a string
 * @returns The string `"<EXPRESSION> = <VALUE>"` where `<EXPRESSION>` is the stringified `expression` parameter
 *      and `<VALUE>` is the value of said expression.
 */
#define TinyLog_debug_expression(expression) (std::string(#expression) + std::string(" = ") + std::to_string(expression))

/**
 * @brief Logs the given message if the given log level is above the current log level, along with any extra strings wanted
 * @param givenLogLevel The log level for this log
 * @param message The message to log
 * @param extras... Any number of strings to be appended to the output
 * @note Automatically fills in the filename and the line number
 * @note The extras argument can be used with the `TinyLog_debug_expression` macro
 * @warning Assumes the logger name is `logger`
 */
#define TinyLog_log(level, message, ...) logger.log(level, message, {__VA_ARGS__}, __FILENAME__, __LINE__)

/**
 * @brief Alternative to the `TinyLog_log_pro` macro, with a custom logger name. See the docs at the `TinyLog_log_pro` macro.
 */
#define TinyLog_logc(logger, level, message, ...) logger.log(level, message, {__VA_ARGS__}, __FILENAME__, __LINE__)
