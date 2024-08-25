#ifndef __ILOGGER_HPP__
# define __ILOGGER_HPP__

# include "define.hpp"
# include "Utils.hpp"
# include <cstdlib>

class ILogger {
    public:
        virtual ~ILogger() = default;
         typedef std::string (ILogger::*LogMethod)(const std::string) const;

        template<typename T1>
        std::string Log(LogMethod logMethod,
            const T1& arg1) {
            std::vector<std::string> args;
            args.push_back(ToString(arg1));
            return (this->*logMethod)(_sformat(args));
        }

        template<typename T1, typename T2>
        std::string Log(LogMethod logMethod,
            const T1& arg1, const T2& arg2) {
            std::vector<std::string> args;
            args.push_back(ToString(arg1));
            args.push_back(ToString(arg2));
            return (this->*logMethod)(_sformat(args));
        }

        template<typename T1, typename T2, typename T3>
        std::string Log(LogMethod logMethod,
            const T1& arg1, const T2& arg2, const T3& arg3) {
            std::vector<std::string> args;
            args.push_back(ToString(arg1));
            args.push_back(ToString(arg2));
            args.push_back(ToString(arg3));
            return (this->*logMethod)(_sformat(args));
        }

        template<typename T1, typename T2, typename T3, typename T4>
        std::string Log(LogMethod logMethod,
            const T1& arg1, const T2& arg2, const T3& arg3, const T4& arg4) {
            std::vector<std::string> args;
            args.push_back(ToString(arg1));
            args.push_back(ToString(arg2));
            args.push_back(ToString(arg3));
            args.push_back(ToString(arg4));
            return (this->*logMethod)(_sformat(args));
        }

        template<typename T1, typename T2, typename T3, typename T4, typename T5>
        std::string Log(LogMethod logMethod,
            const T1& arg1, const T2& arg2, const T3& arg3, const T4& arg4, const T5& arg5) {
            std::vector<std::string> args;
            args.push_back(ToString(arg1));
            args.push_back(ToString(arg2));
            args.push_back(ToString(arg3));
            args.push_back(ToString(arg4));
            args.push_back(ToString(arg5));
            return (this->*logMethod)(_sformat(args));
        }

        template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
        std::string Log(LogMethod logMethod,
            const T1& arg1, const T2& arg2, const T3& arg3,
            const T4& arg4, const T5& arg5, const T6& arg6) {
            std::vector<std::string> args;
            args.push_back(ToString(arg1));
            args.push_back(ToString(arg2));
            args.push_back(ToString(arg3));
            args.push_back(ToString(arg4));
            args.push_back(ToString(arg5));
            args.push_back(ToString(arg6));
            return (this->*logMethod)(_sformat(args));
        }

        template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
        std::string Log(LogMethod logMethod,
            const T1& arg1, const T2& arg2, const T3& arg3,
            const T4& arg4, const T5& arg5, const T6& arg6,
            const T7& arg7) {
            std::vector<std::string> args;
            args.push_back(ToString(arg1));
            args.push_back(ToString(arg2));
            args.push_back(ToString(arg3));
            args.push_back(ToString(arg4));
            args.push_back(ToString(arg5));
            args.push_back(ToString(arg6));
            args.push_back(ToString(arg7));
            return (this->*logMethod)(_sformat(args));
        }

        bool Env(void) {
            const char *value = std::getenv("__ENVIRONMENT__");
            if (value != NULL && std::string(value) == "dev")
                return true;
            return false;
        }

        std::string LogDebug(const std::string args) const {
            const char *value = std::getenv("__ENVIRONMENT__");
            if (value != NULL && std::string(value) == "dev")
                return _logDebug(args);
            else
                return "";
        }

        std::string LogTrace(const std::string args) const {
            const char *value = std::getenv("__ENVIRONMENT__");
            if (value != NULL && std::string(value) == "dev")
                return _logTrace(args);
            else
                return "";
        }

        std::string LogInformation(const std::string args) const {
            return _logInformation(args);
        }

        std::string LogWarning(const std::string args) const {
            return _logWarning(args);
        }

        std::string LogCaution(const std::string args) const {
            return _logCaution(args);
        }

    private:
        void _range(std::ostringstream& os, const std::vector<std::string>& args) {
            for (size_t i = 0; i < args.size(); ++i) {
                os << args[i];
                if (i != args.size() - 1) {
                    os << " ";
                }
            }
        }

        template<typename T>
        std::string ToString(const T& value) {
            std::ostringstream oss;
            oss << value;
            return oss.str();
        }

        std::string _sformat(const std::vector<std::string>& args) {
            std::ostringstream oss;
            _range(oss, args);
            return oss.str();
        }

        virtual std::string _logDebug(std::string args) const = 0;
        virtual std::string _logTrace(std::string args) const = 0;
        virtual std::string _logInformation(std::string args) const = 0;
        virtual std::string _logWarning(std::string args) const = 0;
        virtual std::string _logCaution(std::string args) const = 0;
};

#endif