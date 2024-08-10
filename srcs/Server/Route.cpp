# include "Route.hpp"

RouteResponse    *Route::ProcessRoute(HttpRequest &httpReq) {
    return NULL; // (void)this->ProcessRequest(httpReq);
}

std::string Route::ReturnFileRequest(std::string path) {
    std::string body;

    std::ifstream file(path.c_str());
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            body += line;
        }
        file.close();
    }
    return body;
}

bool Route::FindFilePattern(std::string &path, std::set<std::string> *dirs) {
    std::set<std::string>::iterator it = this->_index.begin();
    for ( ; it != this->_index.end(); ++it) {
        std::set<std::string>::iterator i = dirs->find(*it);
        if (i != dirs->end()) {
            Utils::checkPathEnd(path, *it);
            delete dirs;
            return true;
        }
    }
    return false;
}

// Geters
std::set<std::string>    *Route::GetAllowMethods(void) {
    return this->_allow_methods;
}

std::string Route::GetRedirectPath(void) {
    return this->_redirectPath;
}

std::map<int, std::string>  Route::GetErrorPage(){
    return this->_error_page;
}

std::string                 Route::GetErrorPage(int statusCode) {
    std::map<int, std::string>::iterator it = _error_page.find(statusCode);
    if (it != _error_page.end()) {
        return _error_page[statusCode];
    }
    return "";
}

bool                        Route::IsAllowMethod(std::string method) {
    std::set<std::string>::iterator it = _allow_methods->find(method);
    if (it != _allow_methods->end()) {
        return true;
    }
    return false;
}

int                         Route::GetLimitClientBodySize(void) const {
    return _limit_client_body_size;
}

std::string                 Route::GetRoot(void) const {
    return _root;
}

// Seters
void    Route::SetAllowMethods(std::set<std::string> *methods) {
    this->_allow_methods = methods;
}

void    Route::SetRedirectPath(std::string redirect) {
    this->_redirectPath = redirect;
}


void		Route::SetErrorPageRoute(std::pair<int, std::string> error_page){
    this->_error_page.insert(std::make_pair(error_page.first, error_page.second));
}

std::string                 Route::GetRouteName(void) const {
    return this->_route_name;
}

std::set<std::string>       Route::GetFilesForIndex(void) const {
    return _index;
}

// Base Methods
Route::Route(CommonParameters *server, std::string route_name)  : 
    _allow_methods(server->GetDefaultAllowMethods()),
    _error_page(std::map<int, std::string>()),
    _limit_client_body_size(2048),
    _root(server->GetRoot()),
    _autoIndex(server->GetAutoIndex()),
    _index(server->GetIndex()),
    _stage(R_START)
{
    std::map<std::string, std::string>::iterator it = server->GetReWrites().find(route_name);
    if (it != server->GetReWrites().end())
        this->_redirectPath = server->GetReWrites()[route_name];
    this->_route_name = route_name;
    std::cout << *this;
    (void)this->_limit_client_body_size;
}

Route::Route(IServer *server, std::string route_name, IHandler *handler)  : 
    _allow_methods(server->GetDefaultAllowMethods()),
    _error_page(server->GetDefaultErrorPage()),
    _limit_client_body_size(server->GetLimitClientBodySize()),
    _root(server->GetRoot()),
    _autoIndex(true),
    _index(server->GetIndex()),
    _stage(R_START),
    _handler(handler)
{
    std::map<std::string, std::string>::iterator it = server->GetReWrites().find(route_name);
    if (it != server->GetReWrites().end())
        this->_redirectPath = server->GetReWrites()[route_name];
    this->_route_name = route_name;
    std::cout << *this;
    (void)this->_limit_client_body_size;
}

std::ostream &operator<<(std::ostream &os, Route const &route) {
    switch (route._stage) {
    case R_START:
        os << HYEL "[ Create Route: " << route.GetRouteName() << " ]" reset << std::endl;
        break;
    case R_REQUEST:
        os << HYEL "[ Request sent to route: " << route.GetRouteName() << " ]" reset << std::endl; 
        break;
    default:
        os << "Error";
    }
	return (os);
}

