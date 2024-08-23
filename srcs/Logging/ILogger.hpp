#ifndef __ILOGGER_HPP__
# define __ILOGGER_HPP__

# include "define.hpp"


class ILogger {

    public:
    ~ILogger(void) {}

    virtual void    LogInformation(std::string title, std::string message) = 0;
    virtual void    LogWarning(std::stringstream title, std::stringstream message) = 0;
    virtual void    LogError(std::stringstream title, std::stringstream message) = 0;
};

#endif