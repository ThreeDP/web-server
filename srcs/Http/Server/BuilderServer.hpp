#ifndef __BUILDER_SERVER_HPP__
# define __BUILDER_SERVER_HPP__

#include "IBuilderServer.hpp"

class BuilderServer : public IBuilderServer {
    IHandler        *_handler;
    IServer         *_server;
    ILogger         *_logger;

    public:
    IBuilderRoute   *BuilderRoute;
    BuilderServer(IHandler *handler, ILogger *logger);
    ~BuilderServer(void) {}

    IBuilderServer      &SetupServer(void);
    IBuilderServer      &WithRoute(IRoute *route);
    IBuilderServer      &WithHosts(std::vector<std::string> hosts);
    IBuilderServer      &WithPort(std::string port);
    IBuilderServer      &WithAllowMethods(std::set<std::string> methods);
    IBuilderServer      &WithErrorPages(std::set<HttpStatusCode::Code> statusCodes, std::string filePath);
    IBuilderServer      &WithBodyLimit(int size);
    IBuilderServer      &WithRedirectPath(std::pair<std::string, std::string> pair);
    IBuilderServer      &WithRootDirectory(std::string root);
    IBuilderServer      &WithAutoIndex(bool flag);
    IServer             *GetResult(void);

};

#endif 