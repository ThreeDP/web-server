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
                .WithLocation(path)
                .WithLastModified(path)
                .WithContentType(Utils::GetFileExtension(path))
                .WithFileDescriptor(fd);
            std::cout << _logger->Log(&Logger::LogInformation, "Response By Setup Erro Pages: ", statusCode) << std::endl;
            return (statusCode);
        }
    }
    builder
        .SetupResponse()
        .WithStatusCode(statusCode)
        .WithContentType(".html")
        .WithDefaultPage();
    std::cout << _logger->Log(&Logger::LogInformation, "Response By Default Error: ", statusCode) << std::endl;
    return (statusCode);
}

HttpStatusCode::Code Route::ProcessRequest(
    HttpRequest &request,
    IBuilderResponse &builder
) {
    std::ifstream   *fd = NULL;
    std::string     absolutePath;
    HttpStatusCode::Code result = HttpStatusCode::_DO_NOTHING;
    
    absolutePath = Utils::SanitizePath(this->_root, request.GetPath());
    if (( result = this->_checkAllowMethod(request.GetMethod() ))) { return result; }
    if (( result = this->_checkRedirectPath(this->GetRedirectPath()) )) { return result; }
    if (( result = this->_methods[request.GetMethod()](request) )) { return result; }
    

    if (this->_handler->PathExist(absolutePath))
    {
        bool isDirectory = this->_handler->FileIsDirectory(absolutePath);
        bool allow = this->_handler->IsAllowToGetFile(absolutePath);
        if (allow && isDirectory)
        {
            if (( result = this->_checkExistIndex(request.GetPath(), absolutePath, isDirectory, allow) )) { return result; }
            std::vector<std::string>::iterator it = this->_indexes.begin();
            for ( ; it != this->_indexes.end(); ++it)
            {
                std::string pathAutoindex = absolutePath;
                pathAutoindex = Utils::SanitizePath(pathAutoindex, *it);
                std::cout << pathAutoindex << std::endl;
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
                    std::cout << _logger->Log(&Logger::LogInformation, "Redirect by index: ", HttpStatusCode::_FOUND) << std::endl;
                    return (HttpStatusCode::_FOUND);
                }
            }
            if (this->GetAutoIndex()) {
                DIR *dir = this->_handler->OpenDirectory(absolutePath);
                builder
                    .SetupResponse()
                    .WithStatusCode(HttpStatusCode::_OK)
                    .WithContentType(".html")
                    .WithDirectoryFile(dir, absolutePath);
                std::cout << _logger->Log(&Logger::LogInformation, "Response By Directories: ", HttpStatusCode::_OK) << std::endl;
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
            std::cout << _logger->Log(&Logger::LogInformation, "Response By File Descriptor: ", HttpStatusCode::_OK) << std::endl;
            return (HttpStatusCode::_OK);
        } else if (!allow) {
            return this->_handlerErrorResponse(fd, HttpStatusCode::_FORBIDDEN, builder);
        }
    }
    return this->_handlerErrorResponse(fd, HttpStatusCode::_NOT_FOUND, builder);
}

HttpStatusCode::Code Route::

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

Route::Route(ILogger *logger, IServer *server, IHandler *handler, std::string route_name) : 
    _route_name(route_name),
    _allow_methods(server->GetAllowMethods()),
    _error_page(server->GetErrorPages()),
    _limit_client_body_size(server->GetBodyLimit()),
    _redirectPath(server->GetRedirectPath(route_name)),
    _root(server->GetRootDirectory()),
    _autoIndex(server->GetAutoIndex()),
    _indexes(server->GetPageIndexes()),
    _logger(logger),
    _handler(handler)
{
    _method["GET"] = &Route::Get
    _method["POST"] = &Route::Post
    _method["DELETE"] = &Route::Delete
    _builder = new BuilderResponse(_handler, _logger);
    if (_logger->Env()) {
        std::cerr << _logger->Log(&Logger::LogDebug, "Created Route Class: ") << std::endl;
        std::cerr << _logger->Log(&Logger::LogTrace, "Route Standard Content {\n", this->_toString(), "\n}") << std::endl;
    }
}

Route::~Route(void) {
    std::cerr << _logger->Log(&Logger::LogDebug, "Deleted Route Class.") << std::endl;
}

std::string Route::_toString(void) {
    std::stringstream ss;
    ss << "\t\tRoute Name: " << _route_name << std::endl;
    ss << "\t\tAllow Methods: ";
    for (std::set<std::string>::iterator it = _allow_methods.begin() ; it != _allow_methods.end(); ++it) {
        ss << *it << " ";
    }
    ss << std::endl << "\t\tError Pages: " << std::endl;
    for (std::map<HttpStatusCode::Code, std::string>::iterator it = _error_page.begin(); it != _error_page.end(); ++it) {
        ss << "\t\t\t" << static_cast<int>(it->first) << " " << it->second << std::endl;
    }
    ss << "\t\tBody Limit: " << _limit_client_body_size << std::endl;
    ss << "\t\tRedirect Path: " << _redirectPath << std::endl;
    ss << "\t\tRoot Directory: " << _root  << std::endl;
    ss << "\t\tAuto index: " << std::string((_autoIndex) ? "on" : "off") << std::endl;
    ss << "\t\tindexes: ";
    for (std::vector<std::string>::iterator it = _indexes.begin(); it != _indexes.end(); ++it) {
        ss << *it << " ";
    }
    ss << std::endl;
    return ss.str();
}