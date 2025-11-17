# TinyLog
**TinyLog** is a stupidly simple C++ logging framework, being made as a single header.

## How to use
### Install
Simply put `tinylog.hpp` in your include directory, and you're good to go !

### Use
#### Initialization
Create a root logger and enable string output :
```cpp
// Create a root logger
TinyLog::Logger logger;  // Creates a logger with the default log level for your build type
//TinyLog::Logger logger(TINYLOG_DEFAULT_LOG_LEVEL);  // Equivalent to the line above
//TinyLog::Logger logger(TINYLOG_DEFAULT_DEBUG_MODE_LOG_LEVEL);  // Creates a logger with the default log level being set to the default log level for a debug build
//TinyLog::Logger logger(TinyLog::INFO);  // Creates a logger with the default log level being set to INFO

// Enable string output, with default output to stdout
TinyLog::Logger::enableStringOutput(std::cout);

// Opens a txt log file to output to
std::ofstream logFile("log.txt");

// Adds the txt log file to the list of string outputs
TinyLog::Logger::addStringOutput(logFile);
```

You can omit the `TinyLog` namespace by setting the `TINYLOG_USE_NAMESPACE` macro to `0` before importing TinyLog.

#### Logging basics
Log info text :
```cpp
logger.log(TinyLog::INFO, "Basic text log");
// Outputs : [INFO ] 2025-11-17T12:00:00Z - Basic text log
```
This will only be logged if the log level is lower or equal to INFO.

Log info text with file path and line number :
```cpp
TinyLog_log(TinyLog::INFO, "Basic text log through the macro");
// Outputs : [INFO ] 2025-11-17T12:00:00Z - test/readme_code.cpp (line 40) - Basic text log through the macro
```
This is the recommended method.

#### Logging extras
Log extra information :
```cpp
std::string my_string_variable = "Variable value";
TinyLog_log(TinyLog::DEBUG, "Logging extras", "Extra information here", my_string_variable);
// Outputs : [DEBUG] 2025-11-17T12:00:00Z - test/readme_code.cpp (line 49) - Logging extras - EXTRAS -  Extra information here ; Variable value ;
```

This extra information is made to be used with the `TinyLog_debug_expression(expression)` macro.  
This macro will return a string of the expression and its value.  
```cpp
int a = 5;
TinyLog_log(TinyLog::DEBUG, "Current value of variable a", TinyLog_debug_expression(a));
// Outputs : [DEBUG] 2025-11-17T12:00:00Z - test/readme_code.cpp (line 57) - Current value of variable a - EXTRAS -  a = 5 ;
```

You can add newlines between each extra by setting the `TINYLOG_EXTRAS_ON_SEPARATE_LINES` macro to `1` before importing `TinyLog`.
```cpp
#define TINYLOG_EXTRAS_ON_SEPARATE_LINES 1
#include <tinylog.hpp>

std::string my_string_variable = "Variable value";
TinyLog_log(TinyLog::DEBUG, "Logging extras", "Extra information here", TinyLog_debug_expression(my_string_variable));
/* Outputs : 
[DEBUG] 2025-11-17T12:00:00Z - test/readme_code.cpp (line 67) - Logging extras - EXTRAS :
        - Extra information here ; 
        - my_string_variable = Variable value ;
*/
```

#### Logger inheritance
One of the main features of TinyLog is its logger inheritance chain.  
Simply put, each logger will inherit the log level of the closest parent with a set log level.  

This is useful when working in specific functions, for example.  
You can set the parent logger to the function tree you are working on to `DEBUG` mode, and every log from every child logger and every function call beneath this logger will be logged ; until said parent logger goes out of scope, at which point the log level will return to what it previously was.

Consider the following logger hierarchy :
- INFO
  - INHERIT
    - DEBUG
      - INHERIT
      - FATAL
        - INHERIT
      - INHERIT
        - INHERIT
          - INHERIT
            - WARN
  - WARN
    - INFO
      - INHERIT

The resulting log levels will be observed :
- INFO
  - INFO
    - DEBUG
      - DEBUG
      - FATAL
        - FATAL
      - DEBUG
        - DEBUG
          - DEBUG
            - WARN
  - WARN
    - INFO
      - INFO

You can find such an example of a logger hierarchy in the file `test/test_tinylog.cpp` (see `level1` and `level2` functions).
