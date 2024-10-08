# include "BuilderServer.hpp"
# include "BuilderRoute.hpp"

BuilderServer::BuilderServer(IHandler *handler, ILogger *logger) :
    _handler(handler),
    _logger(logger)
{
    _builderRoute = NULL;
    _server = NULL;
    std::cerr << _logger->Log(&Logger::LogDebug, "Created BuilderServer Class.");
}

BuilderServer::~BuilderServer(void) {
    if (_server != NULL) {
        delete _server;
        _server = NULL;
    }
    if (_builderRoute != NULL) {
        delete _builderRoute;
        _builderRoute = NULL;
    }
    std::cerr << _logger->Log(&Logger::LogDebug, "Deleted BuilderServer Class.");
}

IBuilderServer      &BuilderServer::SetupServer(void) {
    if (_server != NULL) {
        delete _server;
        _server = NULL;
    }
    if (_builderRoute != NULL) {
        delete _builderRoute;
        _builderRoute = NULL;
    }
    _server = new Server(_handler, _logger);
    _builderRoute = new BuilderRoute(_logger, _server, _handler);;
    return *this;
}

IBuilderRoute       *BuilderServer::GetBuilderRoute(void) {
    return _builderRoute;
};

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
        throw std::invalid_argument(_logger->Log(&Logger::LogCaution, "Port Out of band for port."));
    _server->SetPort(port);
    return *this;
}

IBuilderServer      &BuilderServer::WithAllowMethods(std::set<std::string> methods) {
    _server->SetAllowMethods(methods);
    return *this;
}

IBuilderServer      &BuilderServer::WithIndexes(std::vector<std::string> indexes) {
    _server->SetPagesIndexes(indexes);
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
    if (_builderRoute != NULL) {
        delete _builderRoute;
        _builderRoute = NULL;
    }
    if (_logger->Env())
        std::cerr << _logger->Log(&Logger::LogTrace, "GetResult Server: {\n", res->_toString(), "\n}");
    return res;
}