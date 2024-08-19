# include "BuilderRoute.hpp"

BuilderRoute::BuilderRoute(IServer *server, IHandler *handler) {
    _handler = handler;
    _server = server;
    _route = NULL;
}

BuilderRoute::~BuilderRoute(void) {
    if (_route != NULL) {
        delete _route;
        _route = NULL;
    }
}

IBuilderRoute &BuilderRoute::SetupRoute(std::string route_name) {
    if (_route != NULL) {
        delete _route;
        _route = NULL;
    }
    _route = new Route(_server, _handler, route_name);
    return *this;
}

IBuilderRoute &BuilderRoute::WithBodyLimit(int size) {
    _route->SetBodyLimit(size);
    return *this;
}

IBuilderRoute &BuilderRoute::WithAllowMethods(std::set<std::string> methods) {
    _route->SetAllowMethods(methods);
    return *this;
}

IBuilderRoute &BuilderRoute::WithErrorPage(std::set<HttpStatusCode::Code> statusCodes, std::string filePath) {
    _route->SetErrorPage(statusCodes, filePath);
    return *this;
}

IBuilderRoute &BuilderRoute::WithRedirectPath(std::string redirectPath) {
    _route->SetRedirectPath(redirectPath);
    return *this;
}

IBuilderRoute &BuilderRoute::WithRootDirectory(std::string root) {
    _route->SetRootDirectory(root);
    return *this;
}

IBuilderRoute &BuilderRoute::WithPageIndexes(std::vector<std::string> indexes) {
    _route->SetRouteIndexes(indexes);
    return *this;
}

IBuilderRoute &BuilderRoute::WithAutoIndex(bool flag) {
    _route->SetAutoIndex(flag);
    return *this;
}

IRoute        *BuilderRoute::GetResult(void) {
    IRoute *res = _route;
    if (_route != NULL) {
        _route = NULL;
    }
    return res;
}