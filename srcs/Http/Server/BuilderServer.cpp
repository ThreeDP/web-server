# include "BuilderServer.hpp"
# include "BuilderRoute.hpp"

// BuilderServer::BuilderServer(IHandler *handler, ILogger *logger) {
//     _handler = handler;
//     _logger = logger;
//     _server = NULL;
// }

BuilderServer::BuilderServer(IHandler *handler, ILogger *logger) {
    _handler = handler;
    _logger = logger;
    _builderRoute = NULL;
    _server = NULL;
}


IBuilderServer      &BuilderServer::SetupServer(void) {
    if (_server != NULL) {
        _server = NULL;
    }
    _server = new Server(_handler, _logger);
        _builderRoute = new BuilderRoute(_server, _handler);;
    return *this;
}

IBuilderServer      &BuilderServer::StartBRoute(void) {
    if (_builderRoute == NULL)
        _builderRoute = new BuilderRoute(_server, _handler);;
}

IBuilderServer      &BuilderServer::WithRoute(IRoute *route) {
    _server->SetRoute(route->GetRouteName(), route);
    return *this;
}

IBuilderServer      &BuilderServer::WithHosts(std::vector<std::string> hosts) {
    if (!hosts.empty())
        _server->SetHosts(hosts);
    return *this;
}

IBuilderServer      &BuilderServer::WithPort(std::string port) {
    int num;
    std::stringstream ss(port);
    ss >> num;
    if (num < 1 || num > USHRT_MAX)
        throw std::invalid_argument("Out of band for port.");
    _server->SetPort(port);
    return *this;
}

IBuilderServer      &BuilderServer::WithAllowMethods(std::set<std::string> methods) {
    _server->SetAllowMethods(methods);
    return *this;
}

IBuilderServer      &BuilderServer::WithErrorPages(std::set<HttpStatusCode::Code> statusCodes, std::string filePath) {
    _server->SetErrorPage(statusCodes, filePath);
    return *this;
}

IBuilderServer      &BuilderServer::WithBodyLimit(int size) {
    _server->SetBodyLimit(size);
    return *this;
}

IBuilderServer      &BuilderServer::WithRedirectPath(std::pair<std::string, std::string> pair) {
    _server->SetRedirectPath(pair);
    return *this;
}

IBuilderServer      &BuilderServer::WithRootDirectory(std::string root) {
    _server->SetRootDirectory(root);
    return *this;
}

IBuilderServer      &BuilderServer::WithAutoIndex(bool flag) {
    _server->SetAutoIndex(flag);
    return *this;
}

IServer             *BuilderServer::GetResult(void) {
    IServer *res = _server;
    if (_server != NULL) {
        _server = NULL;
    }
    return res;
}