#ifndef __LOGGER_HPP__
# define __LOGGER_HPP__

# include "ILogger.hpp"

class Logger : public ILogger {
    public:
    Logger(void) {}
    ~Logger(void) {}

    void    LogInformation(std::string title, std::string message) {
        std::cout << BLUB" [ " << Utils::getCurrentTimeInGMT() << " ]: " << title << " " << message << reset << std::endl;
    }

    void    LogWarning(std::stringstream title, std::stringstream message) {
        std::cout << YELB" [ " << title.str() << " ]: " reset << message.str() << std::endl;
    }

    void    LogError(std::stringstream title, std::stringstream message) {
        std::cerr << REDB" [ " << title.str() << " ]: " reset << message.str() << std::endl;
    }
};

#endif