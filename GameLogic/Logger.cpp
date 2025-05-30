#include "Logger.hpp"
#include <iostream>

namespace coup {

    void Logger::log(const std::string& message) {
        std::cout << "[LOG] " << message << std::endl;
    }

}
