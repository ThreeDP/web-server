#ifndef __IBUILDER_SERVER_HPP__
# define __IBUILDER_SERVER_HPP__

# include "Route.hpp"
# include "Logger.hpp"
# include "Server.hpp"

class IBuilderServer {
    public:
    virtual ~IBuilderServer(void) {}

    virtual IBuilderServer      &SetupServer(void) = 0;
    virtual IBuilderServer      &WithRoute(IRoute *route) = 0;
    virtual IBuilderServer      &WithHosts(std::vector<std::string> hosts) = 0;
    virtual IBuilderServer      &WithPort(std::string port) = 0;

    virtual IBuilderServer      &WithAllowMethods(std::set<std::string> methods) = 0;
    virtual IBuilderServer      &WithErrorPages(std::set<HttpStatusCode::Code> statusCode, std::string filePath) = 0;
    virtual IBuilderServer      &WithBodyLimit(int size) = 0;
    virtual IBuilderServer      &WithRedirectPath(std::pair<std::string, std::string> pair) = 0;
    virtual IBuilderServer      &WithRootDirectory(std::string root) = 0;
    virtual IBuilderServer      &WithAutoIndex(bool flag) = 0;

    virtual IServer             *GetResult(void) = 0;
};

#endif