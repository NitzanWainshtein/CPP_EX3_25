// Email: nitzanwa@gmail.com

#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>

namespace coup {

    /**
     * @class Logger
     * @brief Simple logging utility for game events
     * 
     * Provides static method for logging game events to console
     * Can be extended to support file logging or different log levels
     */
    class Logger {
    public:
        /**
         * @brief Log a message to console
         * @param message Message to log
         */
        static void log(const std::string& message);
    };

}

#endif // LOGGER_HPP