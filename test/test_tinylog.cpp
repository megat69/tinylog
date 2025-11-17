#include <iostream>
#include <fstream>

#define TINYLOG_EXTRAS_ON_SEPARATE_LINES 1
#include <tinylog.hpp>

void level2() {
    TinyLog::Logger logger(TinyLog::INHERIT);
    logger.log(TinyLog::DEBUG, "Level 2", {}, __FILENAME__, __LINE__);
}

void level1() {
    TinyLog::Logger logger(TinyLog::DEBUG);
    TinyLog_log(TinyLog::DEBUG, "Level 1");

    level2();
}

int main() {
    // Logger setup
    TinyLog::Logger logger;
    TinyLog::Logger::enableStringOutput(std::cout);
    std::ofstream logFile("log.txt");
    TinyLog::Logger::addStringOutput(logFile);

    // Logger tests
    logger.log(TinyLog::INFO, "Hello debug users :D");
    TinyLog_logc(logger, TinyLog::ERROR, "Hello all users :D");

    int a = 5;
    TinyLog_log(TinyLog::FATAL, "Debugging an expression", TinyLog_debug_expression(a), TinyLog_debug_expression(a == 5), "Extra string");

    level1();

    return 0;
}