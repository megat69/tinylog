#include <iostream>

#include <tinylog.hpp>

void level2() {
    TinyLog::Logger logger(TinyLog::INHERIT);
    logger.log(TinyLog::DEBUG, "Level 2");
}

void level1() {
    TinyLog::Logger logger(TinyLog::DEBUG);
    logger.log(TinyLog::DEBUG, "Level 1");

    level2();
}

int main() {
    TinyLog::Logger logger;
    TinyLog::Logger::enableStringOutput(std::cout);
    logger.log(TinyLog::INFO, "Hello debug users :D");
    logger.log(TinyLog::ERROR, "Hello all users :D");

    level1();

    return 0;
}