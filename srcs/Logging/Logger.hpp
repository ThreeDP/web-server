#ifndef __LOGGER_HPP__
# define __LOGGER_HPP__

#include <sstream>
#include <string>
# include "ILogger.hpp"

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

// void Range(std::ostringstream& os, const std::vector<std::string>& args) {
//     for (size_t i = 0; i < args.size(); ++i) {
//         os << args[i];
//         if (i != args.size() - 1) {
//             os << " ";
//         }
//     }
// }

// template<typename T>
// std::string ToString(const T& value) {
//     std::ostringstream oss;
//     oss << value;
//     return oss.str();
// }

// std::string sformat(const std::vector<std::string>& args) {
//     std::ostringstream oss;
//     Range(oss, args);
//     return oss.str();
// }

// template<typename T1>
// std::string sformat(const T1& arg1) {
//     std::vector<std::string> args;
//     args.push_back(ToString(arg1));
//     return sformat(args);
// }

// template<typename T1, typename T2>
// std::string sformat(const T1& arg1, const T2& arg2) {
//     std::vector<std::string> args;
//     args.push_back(ToString(arg1));
//     args.push_back(ToString(arg2));
//     return sformat(args);
// }

// template<typename T1, typename T2, typename T3>
// std::string sformat(const T1& arg1, const T2& arg2, const T3& arg3) {
//     std::vector<std::string> args;
//     args.push_back(ToString(arg1));
//     args.push_back(ToString(arg2));
//     args.push_back(ToString(arg3));
//     return sformat(args);
// }

// template<typename T1, typename T2, typename T3, typename T4>
// std::string sformat(const T1& arg1, const T2& arg2, const T3& arg3, const T4& arg4) {
//     std::vector<std::string> args;
//     args.push_back(ToString(arg1));
//     args.push_back(ToString(arg2));
//     args.push_back(ToString(arg3));
//     args.push_back(ToString(arg4));
//     return sformat(args);
// }

// template<typename T1, typename T2, typename T3, typename T4, typename T5>
// std::string sformat(const T1& arg1, const T2& arg2, const T3& arg3, const T4& arg4, const T5& arg5) {
//     std::vector<std::string> args;
//     args.push_back(ToString(arg1));
//     args.push_back(ToString(arg2));
//     args.push_back(ToString(arg3));
//     args.push_back(ToString(arg4));
//     args.push_back(ToString(arg5));
//     return sformat(args);
// }

// template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
// std::string sformat(const T1& arg1, const T2& arg2, const T3& arg3, const T4& arg4, const T5& arg5, const T6& arg6) {
//     std::vector<std::string> args;
//     args.push_back(ToString(arg1));
//     args.push_back(ToString(arg2));
//     args.push_back(ToString(arg3));
//     args.push_back(ToString(arg4));
//     args.push_back(ToString(arg5));
//     args.push_back(ToString(arg6));
//     return sformat(args);
// }

// template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
// std::string sformat(const T1& arg1, const T2& arg2, const T3& arg3, const T4& arg4, const T5& arg5, const T6& arg6, const T7& arg7) {
//     std::vector<std::string> args;
//     args.push_back(ToString(arg1));
//     args.push_back(ToString(arg2));
//     args.push_back(ToString(arg3));
//     args.push_back(ToString(arg4));
//     args.push_back(ToString(arg5));
//     args.push_back(ToString(arg6));
//     args.push_back(ToString(arg7));
//     return sformat(args);
// }

// template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
// std::string sformat(const T1& arg1, const T2& arg2, const T3& arg3, const T4& arg4, const T5& arg5, const T6& arg6, const T7& arg7, const T8& arg8) {
//     std::vector<std::string> args;
//     args.push_back(ToString(arg1));
//     args.push_back(ToString(arg2));
//     args.push_back(ToString(arg3));
//     args.push_back(ToString(arg4));
//     args.push_back(ToString(arg5));
//     args.push_back(ToString(arg6));
//     args.push_back(ToString(arg7));
//     args.push_back(ToString(arg8));
//     return sformat(args);
// }

// template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
// std::string sformat(const T1& arg1, const T2& arg2, const T3& arg3, const T4& arg4, const T5& arg5, const T6& arg6, const T7& arg7, const T8& arg8, const T9& arg9) {
//     std::vector<std::string> args;
//     args.push_back(ToString(arg1));
//     args.push_back(ToString(arg2));
//     args.push_back(ToString(arg3));
//     args.push_back(ToString(arg4));
//     args.push_back(ToString(arg5));
//     args.push_back(ToString(arg6));
//     args.push_back(ToString(arg7));
//     args.push_back(ToString(arg8));
//     args.push_back(ToString(arg9));
//     return sformat(args);
// }

// template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10>
// std::string sformat(const T1& arg1, const T2& arg2, const T3& arg3, const T4& arg4, const T5& arg5, const T6& arg6, const T7& arg7, const T8& arg8, const T9& arg9, const T10& arg10) {
//     std::vector<std::string> args;
//     args.push_back(ToString(arg1));
//     args.push_back(ToString(arg2));
//     args.push_back(ToString(arg3));
//     args.push_back(ToString(arg4));
//     args.push_back(ToString(arg5));
//     args.push_back(ToString(arg6));
//     args.push_back(ToString(arg7));
//     args.push_back(ToString(arg8));
//     args.push_back(ToString(arg9));
//     args.push_back(ToString(arg10));
//     return sformat(args);
// }

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