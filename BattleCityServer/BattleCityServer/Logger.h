#pragma once
#include <iostream>
#include <string>
#include <sstream>

class Logger {
public:
    template<typename... Args>
    static void Log(Args&&... args) {
        std::ostringstream oss;
        (oss << ... << args); 
        std::cout << std::endl << "[LOG]: " << oss.str() << std::endl;
    }
};
