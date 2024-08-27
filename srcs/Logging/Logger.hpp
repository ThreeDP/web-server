#ifndef __LOGGER_HPP__
# define __LOGGER_HPP__

#include <sstream>
#include <string>
# include "ILogger.hpp"

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

class Logger : public ILogger {
    public:
        Logger(void) : ILogger() {

        }
        
        ~Logger(void) {

        }
    
    private:
        std::string _logDebug(std::string args) const {
            std::ostringstream oss;
            oss <<  BYEL "[ Debug - " <<Utils::getCurrentTimeInGMT() << " ]: ";
            oss << args << __CODE_RESET__;
            return oss.str();
        }

        std::string _logTrace(std::string args) const {
            std::stringstream oss;
            oss <<  BWHT "[ Trace - " <<Utils::getCurrentTimeInGMT() << " ]: ";
            oss << args << __CODE_RESET__;
            return oss.str();
        }

        std::string _logInformation(std::string args) const {
            std::ostringstream oss;
            oss <<  BLUHB "[ Info - " <<Utils::getCurrentTimeInGMT() << " ]: ";
            oss << args << __CODE_RESET__;
            return oss.str();
        }

        std::string _logWarning(std::string args) const {
            std::ostringstream oss;
            oss <<  YELHB "[ Warning - " <<Utils::getCurrentTimeInGMT() << " ]: ";
            oss << args << __CODE_RESET__;
            return oss.str();
        }

        std::string _logCaution(std::string args) const {
            std::ostringstream oss;
            oss <<  REDHB "[ Caution - " <<Utils::getCurrentTimeInGMT() << " ]: ";
            oss << args << __CODE_RESET__;
            return oss.str();
        }
};

#endif