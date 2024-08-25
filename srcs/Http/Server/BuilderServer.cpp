# include "BuilderServer.hpp"
# include "BuilderRoute.hpp"

BuilderServer::BuilderServer(ILogger *logger, IHandler *handler) :
    _handler(handler),
    _logger(logger)
{
    _builderRoute = NULL;
    _server = NULL;
    std::cerr << _logger->Log(&Logger::LogDebug, "Created BuilderServer Class.") << std::endl;
}

BuilderServer::~BuilderServer(void) {
    std::cerr << _logger->Log(&Logger::LogDebug, "Deleted BuilderServer Class.") << std::endl;
}

IBuilderServer      &BuilderServer::SetupServer(void) {
    if (_server != NULL) {
        _server = NULL;
    }
    _server = new Server(_handler, _logger);
    _builderRoute = new BuilderRoute(_logger, _server, _handler);
    return *this;
}

 IBuilderRoute       *BuilderServer::GetBuilderRoute(void) {
    return this->_builderRoute;
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
    std::cout << _logger->Log(&Logger::LogInformation, "Get Server") << std::endl;
    if (_logger->Env())
        // std::cout << _logger->Log(&Logger::LogTrace, "GetResult Server: {\n", res->_toString(), "\n}") << std::endl;
    return res;
}