#ifndef __BUILDER_SERVER_HPP__
# define __BUILDER_SERVER_HPP__

#include "IBuilderServer.hpp"

class BuilderServer : public IBuilderServer {
    IHandler        *_handler;
    IServer         *_server;

    public:
    BuilderServer(IHandler *handler) {
        _handler = handler;
        _server = NULL;
    }

    ~BuilderServer(void) {}

    virtual IBuilderServer      &SetupServer(void) {
        if (_server != NULL) {
            _server = NULL;
        }
        _server = new Server(_handler);
        return *this;
    }

    virtual IBuilderServer      &WithRoute(IRoute *route) {
        _server->SetRoute(route->GetRouteName(), route);
        return *this;
    }

    virtual IBuilderServer      &WithHosts(std::vector<std::string> hosts) {
        _server->SetHosts(hosts);
        return *this;
    }

    virtual IBuilderServer      &WithPort(std::string port) {
        _server->SetPort(port);
        return *this;
    }

    virtual IBuilderServer      &WithAllowMethods(std::set<std::string> methods) {
        _server->SetAllowMethods(methods);
        return *this;
    }

    virtual IBuilderServer      &WithErrorPages(std::set<HttpStatusCode::Code> statusCodes, std::string filePath) {
        _server->SetErrorPage(statusCodes, filePath);
        return *this;
    }

    virtual IBuilderServer      &WithbodyLimit(int size) {
        _server->SetBodyLimit(size);
        return *this;
    }

    virtual IBuilderServer      &WithRedirectPath(std::pair<std::string, std::string> pair) {
        _server->SetRedirectPath(pair);
        return *this;
    }

    virtual IBuilderServer      &WithRootDirectory(std::string root) {
        _server->SetRootDirectory(root);
        return *this;
    }

    virtual IBuilderServer      &WithAutoIndex(bool flag) {
        _server->SetAutoIndex(flag);
        return *this;
    }

    virtual IServer             *GetResult(void) {
        IServer *res = _server;
        if (_server != NULL) {
            _server = NULL;
        }
        return res;
    }
};

#endif 