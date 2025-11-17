# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.5.1] - 2025-11-17

### [Changed]

- Refactored the `Logger.log()` method to be more concise.

## [0.5.0] - 2025-11-17

### [Added]

- `Logger::setLoggerChainCapacity(size_t capacity)` : pre-allocates a given amount of loggers in the chain ; set to expected maximum function call depth.

## [0.4.0] - 2025-11-17

### [Added]

- Extra strings to be displayed in the logs using the `extras` system, helping understand the debug better
- `TINYLOG_EXTRAS_ON_SEPARATE_LINES` macro, adds separate lines between each extra if set to 1. 0 by default.
- `TinyLog_debug_expression` macro, returns a string of the expression and its value, helps with debugging/logging state

### [Changed]

- Each log function/macro can now accept `extras`

## [0.3.0] - 2025-11-16

### [Added]
- Timestamps in logs

## [0.2.0] - 2025-11-16

### [Added]
- `SOURCE_PATH_SIZE` macro with the size of the absolute path to the project root. Defined in `CMakeLists.txt`.
- `__FILENAME__` macro, with the path of the current file relative to the project root **IF** `SOURCE_PATH_SIZE`, otherwise just maps to the `__FILE__` macro.
- `TinyLog_log` and `TinyLog_logc` macros for easier logging

### [Changed]
- Added more info to each log

## [0.1.0] - 2025-11-16

### [Added]
- `Logger::addStringOutput(std::ostream&)` added
- Added the file `log.txt` to `.gitignore`

### [Changed]

- Multiple output streams instead of just one
  - `Logger::addStringOutput(std::ostream&)` added
  - Turned `Logger::loggers` into an `std::vector` of `std::ostream*`

## [0.0.1] - 2025-11-16

### [Added]

- Basics of the library