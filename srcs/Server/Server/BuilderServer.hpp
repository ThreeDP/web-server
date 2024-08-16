#ifndef __BUILDER_SERVER_HPP__
# define __BUILDER_SERVER_HPP__

#include "IBuilderServer.hpp"

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
        _server = Server(handler);
        *this;
    }

    virtual IBuilderServer      &WithRoute(IRoute *route) {
        _server->SetRoute(route->GetRouteName(), route);
        *this;
    }

    virtual IBuilderServer      &WithHosts(std::vector<std::string> hosts) {
        _server->SetHosts(hosts);
        *this;
    }

    virtual IBuilderServer      &WithPort(std::string port) {
        _server->SetPort(port);
        *this;
    }

    virtual IBuilderServer      &WithAllowMethods(std::set<std::string> methods) {
        _server->SetAllowMethods(methods);
        *this;
    }

    virtual IBuilderServer      &WithErrorPages(std::set<HttpStatusCode::Code> statusCodes, std::string filePath) {
        _server->SetErrorPage(statusCodes, filePath);
        *this;
    }

    virtual IBuilderServer      &WithbodyLimit(int size) {
        _server->SetBodyLimit(size);
        *this;
    }

    virtual IBuilderServer      &WithRedirectPath(std::pair<std::string, std::string> pair) {
        _server->SetRedirectPath(pair);
        *this;
    }

    virtual IBuilderServer      &WithRootDirectory(std::string root) {
        _server->SetRootDirectory(root);
        *this;
    }

    virtual IBuilderServer      &WithAutoIndex(bool flag) {
        _server->SetAutoIndex(flag);
        *this;
    }

    virtual IServer             *GetResult(void) {
        IServer *res = _server;
        if (_server != NULL) {
            _server = NULL;
        }
        return res;
    }
}

#endif 