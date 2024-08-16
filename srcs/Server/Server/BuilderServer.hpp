#ifndef __BUILDER_SERVER_HPP__
# define __BUILDER_SERVER_HPP__

class BuilderServer {
    IHandler        *_handler;
    IServer         *_server;

    BuilderServer(IHandler handler) {
        _handler = handler;
        _server = NULL;
    }
    ~BuilderServer(void) {}

    virtual IBuilderServer      &SetupServer(void) {
        if (_server != NULL) {
            _server = NULL;
        }
        _server = Server();
    }
    virtual IBuilderServer      &WithRoute(IRoute *route) {
        _server->SetRoute(route->GetRouteName(), route);
    }

    virtual IBuilderServer      &WithHosts(std::vector<std::string> hosts) {
        
    }

    virtual IBuilderServer      &WithPort(std::string port) = 0;

    virtual IBuilderServer      &WithAllowMethods(std::set<std::string> methods) = 0;
    virtual IBuilderServer      &WithErrorPages(std::set<HttpStatusCode::Code> statusCode, std::string filePath) = 0;
    virtual IBuilderServer      &WithbodyLimit(int size) = 0;
    virtual IBuilderServer      &WithRedirectPath(std::pair<std::string, std::string> pair) = 0;
    virtual IBuilderServer      &WithRootDirectory(std::string root) = 0;
    virtual IBuilderServer      &WithAutoIndex(bool flag) = 0;

    virtual IServer             *GetResult(void) = 0;

}

#endif 