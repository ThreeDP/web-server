# include "Route.hpp"

HttpStatusCode::Code Route::_handlerErrorResponse(
    std::ifstream *fd,
    HttpStatusCode::Code statusCode,
    IBuilderResponse &builder
) {
    std::string path;

    bool hasErrorPage = (path = this->GetErrorPage(statusCode)) != "";
    path = Utils::SanitizePath(this->_root, path);
    if (hasErrorPage && this->_handler->FileExist(path)) {
        if (this->_handler->IsAllowToGetFile(path)) {
            fd = this->_handler->OpenFile(path);
            builder
                .SetupResponse()
                .WithStatusCode(statusCode)
                .WithFileDescriptor(fd)
                .WithContentType(Utils::GetFileExtension(path));
            return (statusCode);
        }
    }
    builder
        .SetupResponse()
        .WithStatusCode(statusCode)
        .WithContentType(".html")
        .WithDefaultPage();
    return (statusCode);
}

HttpStatusCode::Code Route::ProcessRequest(
    HttpRequest &request,
    IBuilderResponse &builder
) {
    std::ifstream   *fd = NULL;
    std::string     absolutePath;
    
    absolutePath = Utils::SanitizePath(this->_root, request.GetPath());
    if (!this->IsAllowMethod(request.GetMethod())) {
        return this->_handlerErrorResponse(
            fd,
            HttpStatusCode::_METHOD_NOT_ALLOWED,
            builder
        );
    } else if (this->_limit_client_body_size < request.GetBodySize()) {
        return this->_handlerErrorResponse(
            fd,
            HttpStatusCode::_CONTENT_TOO_LARGE,
            builder
        );
    } else if (this->GetRedirectPath() != "") {
        builder
            .SetupResponse()
            .WithStatusCode(HttpStatusCode::_PERMANENT_REDIRECT)
            .WithLocation("/" + this->GetRedirectPath())
            .WithDefaultPage();
        return (HttpStatusCode::_PERMANENT_REDIRECT);
    }
    if (this->_handler->PathExist(absolutePath))
    {
        bool isDirectory = this->_handler->FileIsDirectory(absolutePath);
        bool allow = this->_handler->IsAllowToGetFile(absolutePath);
        if (allow && isDirectory)
        {
            std::string pathAutoindex = absolutePath;
            for (std::set<std::string>::iterator it = this->_index.begin(); it != this->_index.end(); ++it)
            {
                pathAutoindex = Utils::SanitizePath(pathAutoindex, *it);
                if (this->_handler->PathExist(pathAutoindex)
                && this->_handler->IsAllowToGetFile(pathAutoindex)
                && !this->_handler->FileIsDirectory(pathAutoindex))
                {
                    builder
                        .SetupResponse()
                        .WithStatusCode(HttpStatusCode::_FOUND)
                        .WithLocation(Utils::SanitizePath("http://localhost:8081", Utils::SanitizePath(request.GetPath(), *it)))
                        .WithDefaultPage();
                    return (HttpStatusCode::_FOUND);
                }
            }
            if (this->_autoIndex) {
                DIR *dir = this->_handler->OpenDirectory(absolutePath);
                builder
                    .SetupResponse()
                    .WithStatusCode(HttpStatusCode::_OK)
                    .WithDirectoryFile(dir, absolutePath)
                    .WithContentType(".html");
                return (HttpStatusCode::_OK);
            }
        } else if (allow) {
            fd = this->_handler->OpenFile(absolutePath);
            builder
                .SetupResponse()
                .WithStatusCode(HttpStatusCode::_OK)
                .WithContentType(Utils::GetFileExtension(absolutePath))
                .WithFileDescriptor(fd);
            return (HttpStatusCode::_OK);
        } else if (!allow) {
            return this->_handlerErrorResponse(fd, HttpStatusCode::_FORBIDDEN, builder);
        }
    }
    return this->_handlerErrorResponse(fd, HttpStatusCode::_NOT_FOUND, builder);
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

