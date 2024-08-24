# include "Route.hpp"

HttpStatusCode::Code Route::_handlerErrorResponse(
    std::ifstream *fd,
    HttpStatusCode::Code statusCode,
    IBuilderResponse &builder
) {
    std::string path;

    bool hasErrorPage = (path = this->GetErrorPage(statusCode)) != "";
    std::string abspath = Utils::SanitizePath(this->_root, this->_route_name);
    path = Utils::SanitizePath(abspath, path);
    if (hasErrorPage && this->_handler->FileExist(path)) {
        if (this->_handler->IsAllowToGetFile(path)) {
            fd = this->_handler->OpenFile(path);
            builder
                .SetupResponse()
                .WithStatusCode(statusCode)
                .WithFileDescriptor(fd)
                .WithLastModified(path)
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
    } else if (this->GetBodyLimit() < request.GetBodySize()) {
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
            std::vector<std::string>::iterator it = this->_indexes.begin();
            for ( ; it != this->_indexes.end(); ++it)
            {
                pathAutoindex = Utils::SanitizePath(pathAutoindex, *it);
                if (this->_handler->PathExist(pathAutoindex)
                && this->_handler->IsAllowToGetFile(pathAutoindex)
                && !this->_handler->FileIsDirectory(pathAutoindex))
                {
                    builder
                        .SetupResponse()
                        .WithStatusCode(HttpStatusCode::_FOUND)
                        .WithLocation(Utils::SanitizePath("http://localhost:8081",
                            Utils::SanitizePath(request.GetPath(), *it)))
                        .WithDefaultPage();
                    return (HttpStatusCode::_FOUND);
                }
            }
            if (this->GetAutoIndex()) {
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
                .WithLastModified(absolutePath)
                .WithContentType(Utils::GetFileExtension(absolutePath))
                .WithFileDescriptor(fd);
            return (HttpStatusCode::_OK);
        } else if (!allow) {
            return this->_handlerErrorResponse(fd, HttpStatusCode::_FORBIDDEN, builder);
        }
    }
    return this->_handlerErrorResponse(fd, HttpStatusCode::_NOT_FOUND, builder);
}

// Set Methods
void        Route::SetRouteName(std::string routeName) {
    this->_route_name = routeName;
}

void        Route::SetBodyLimit(int size) {
    this->_limit_client_body_size = size;
}

void        Route::SetAllowMethods(std::set<std::string> methods) {
    this->_allow_methods.clear();
    std::set<std::string>::iterator it = methods.begin();
    for ( ; it != methods.end(); ++it) {
        this->_allow_methods.insert(*it);
    }
}

void        Route::SetErrorPage(std::set<HttpStatusCode::Code> statusCodes, std::string filePath) {
    this->_error_page.clear();
    std::set<HttpStatusCode::Code>::iterator it = statusCodes.begin();
    for ( ; it != statusCodes.end(); ++it) {
        this->_error_page[*it] = filePath;
    }
}

void        Route::SetRedirectPath(std::string redirectPath) {
    this->_redirectPath = redirectPath;
}

void        Route::SetRootDirectory(std::string root) {
    this->_root = root;
}

void        Route::SetRouteIndexes(std::vector<std::string> indexes) {
    this->_indexes.clear();
    std::vector<std::string>::iterator it = indexes.begin();
    for ( ; it != indexes.end(); ++it) {
        this->_indexes.push_back(*it);
    }
}

void        Route::SetAutoIndex(bool flag) {
    this->_autoIndex = flag;
}

// Geters
std::string Route::GetRouteName(void) {
    return this->_route_name;
}

int Route::GetBodyLimit(void) {
    return this->_limit_client_body_size;
}

bool    Route::IsAllowMethod(std::string method) {
    std::set<std::string>::iterator it = _allow_methods.find(method);
    if (it != _allow_methods.end()) {
        return true;
    }
    return false;
}

std::string Route::GetErrorPage(HttpStatusCode::Code code) {
    std::map<HttpStatusCode::Code, std::string>::iterator it = _error_page.find(code);
    if (it != _error_page.end()) {
        return _error_page[code];
    }
    return "";
}

std::string Route::GetRedirectPath(void) {
    return this->_redirectPath;
}

std::string Route::GetRootDirectory(void) {
    return this->_root;
}

const std::vector<std::string> Route::GetRouteIndexes(void) {
    return this->_indexes;
}

bool Route::GetAutoIndex(void) {
    return this->_autoIndex;
}

Route::Route(ILogger *logger, IServer *server, IHandler *handler, std::string route_name)  : 
    _allow_methods(server->GetAllowMethods()),
    _error_page(server->GetErrorPages()),
    _limit_client_body_size(server->GetBodyLimit()),
    _root(server->GetRootDirectory()),
    _autoIndex(server->GetAutoIndex()),
    _indexes(server->GetPageIndexes()),
    _stage(R_START),
    _redirectPath(server->GetRedirectPath(route_name)),
    _handler(handler),
    _logger(logger)
{
    // std::map<std::string, std::string>::iterator it = server->GetReWrites().find(route_name);
    // if (it != server->GetReWrites().end())
    //     this->_redirectPath = server->GetReWrites()[route_name];  
    // _indexes.push_back("index.html");
    // _error_page[HttpStatusCode::_NOT_FOUND] = "404.html";
    // _allow_methods.insert("GET");
}

Route::~Route(void) {
    
}