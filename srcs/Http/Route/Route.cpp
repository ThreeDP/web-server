# include "Route.hpp"

/**!
 * 
 * Init Method:
 * Route processes a request
 * 
 */

IHttpResponse *Route::ProcessRequest(HttpRequest &request) {
    std::string     absolutePath;
    absolutePath = Utils::SanitizePath(this->_root, request.GetPath());
    if (request._flag != HttpStatusCode::_DO_NOTHING) {
        _errorHandler(request._flag);
        return _builder->GetResult(); 
    }
    if (request.GetHTTPVersion() != "HTTP/1.1") {
        _errorHandler(HttpStatusCode::_HTTP_VERSION_NOT_SUPPORTED);
        return _builder->GetResult(); 
    }
    if (request._payload.find("Content-Length:") == request._payload.end()) {
        _errorHandler(HttpStatusCode::_LENGTH_REQUIRED);
        return _builder->GetResult(); 
    }
    if (request.GetPath().size() + request.GetQueryParams().size() > 4096) {
        _errorHandler(HttpStatusCode::_URI_TOO_LONG);
        return _builder->GetResult(); 
    }
    if (this->_checkAllowMethod(request.GetMethod())) {
        return _builder->GetResult(); 
    }
    if (this->_checkRedirectPath(this->GetRedirectPath())) {
        return _builder->GetResult();
    }
    if (request.GetMethod() == "GET") {
        if (this->Get( request, absolutePath) == HttpStatusCode::_CGI) {
            return NULL;
        }
        return _builder->GetResult();
    } else if (request.GetMethod() == "POST") {
        if (this->Post( request, absolutePath) == HttpStatusCode::_CGI) {
            return NULL;
        }
        return _builder->GetResult();
    } else if (request.GetMethod() == "DELETE") {
        if (this->Delete( request, absolutePath) == HttpStatusCode::_CGI) {
            return NULL;
        }
        return _builder->GetResult();
    }
    _errorHandler(HttpStatusCode::_INTERNAL_SERVER_ERROR);
    std::cout << _logger->Log(&Logger::LogInformation, "Route Not Found or Configurated", HttpStatusCode::_INTERNAL_SERVER_ERROR);
    return _builder->GetResult();
}

/**!
 * 
 * General Assets
 * Use in all http methods GET, POST and DELETE.
 * 
 */

HttpStatusCode::Code Route::_checkAllowMethod(std::string method) {
    HttpStatusCode::Code status = HttpStatusCode::_METHOD_NOT_ALLOWED;
    HttpStatusCode::Code result = HttpStatusCode::_DO_NOTHING;
    if (Utils::SanitizeOneMethod(method)) {
        if ((result = this->_errorHandlerWithFile(HttpStatusCode::_NOT_IMPLEMENTED))) { return result; }
        return this->_errorHandlerDefault(HttpStatusCode::_NOT_IMPLEMENTED);
    }
    if (!this->IsAllowMethod(method)) {

        if ((result = this->_errorHandlerWithFile(status))) { return result; }
        return this->_errorHandlerDefault(status);
    }
    return HttpStatusCode::_DO_NOTHING;
}

HttpStatusCode::Code Route::_checkRedirectPath(std::string path) {
    HttpStatusCode::Code result = HttpStatusCode::_DO_NOTHING;

    if (path != "") {
        result = HttpStatusCode::_PERMANENT_REDIRECT;

        _builder
            ->SetupResponse()
            .WithStatusCode(result)
            .WithLocation(Utils::SanitizePath("", path))
            .WithDefaultPage();
        std::cout << _logger->Log(&Logger::LogInformation, "Permanent Redirect: ", HttpStatusCode::_PERMANENT_REDIRECT);
        return (result);
    }
    return result;
}

HttpStatusCode::Code Route::_checkBodyLimit(size_t limit) {
    if (this->GetBodyLimit() < limit) {
        HttpStatusCode::Code status = HttpStatusCode::_CONTENT_TOO_LARGE;
        HttpStatusCode::Code result = HttpStatusCode::_DO_NOTHING;
        
        if ((result = this->_errorHandlerWithFile(status))) { return result; }
        return this->_errorHandlerDefault(status);
    }
    return HttpStatusCode::_DO_NOTHING;
}

HttpStatusCode::Code Route::_checkDirectory(std::string absPath, HttpRequest &request) {
    if (( absPath[absPath.length() - 1] != '/')) {
        _builder->SetupResponse()
            .WithContentType(".html")
            .WithStatusCode(HttpStatusCode::_TEMPORARY_REDIRECT)
            .WithLocation(Utils::SanitizePath("/", request.GetPath() + "/"))
            .WithDefaultPage();
            return HttpStatusCode::_TEMPORARY_REDIRECT;
    }
    return HttpStatusCode::_DO_NOTHING;
}

HttpStatusCode::Code Route::_checkAutoIndex(std::string absolutePath) {
    if (this->GetAutoIndex()) {
        DIR *dir = this->_handler->OpenDirectory(absolutePath);
        _builder
            ->SetupResponse()
            .WithStatusCode(HttpStatusCode::_OK)
            .WithContentType(".html")
            .WithDirectoryFile(dir, absolutePath);
        std::cout << _logger->Log(&Logger::LogInformation, "Response By Directories: ", HttpStatusCode::_OK);
        return (HttpStatusCode::_OK);
    }
    return HttpStatusCode::_DO_NOTHING;
}

HttpStatusCode::Code Route::_errorHandlerWithFile(HttpStatusCode::Code statusCode) {
    std::string path;
    std::ifstream *fd = NULL;

    bool hasErrorPage = (path = this->GetErrorPage(statusCode)) != "";
    std::string abspath = Utils::SanitizePath(this->_root, this->_route_name);
    path = Utils::SanitizePath(abspath, path);
    if (hasErrorPage && this->_handler->FileExist(path)) {
        if (this->_handler->IsAllowToGetFile(path)) {
            fd = this->_handler->OpenFile(path);
            _builder
                ->SetupResponse()
                .WithStatusCode(statusCode)
                .WithLocation(path)
                .WithLastModified(path)
                .WithContentType(Utils::GetFileExtension(path))
                .WithFileDescriptor(fd);
            std::cout << _logger->Log(&Logger::LogInformation, "Response By Setup Erro Pages: ", statusCode);
            return (statusCode);
        }
    }
    return HttpStatusCode::_DO_NOTHING;
}

HttpStatusCode::Code Route::_errorHandlerDefault(HttpStatusCode::Code statusCode) {
    _builder
        ->SetupResponse()
        .WithStatusCode(statusCode)
        .WithContentType(".html")
        .WithDefaultPage();
    
    std::cout << _logger->Log(&Logger::LogInformation, "Response By Default Error: ", statusCode);
    return (statusCode);
}

HttpStatusCode::Code Route::_checkActionInFile(std::string absolutePath) {
    std::ifstream *fd = this->_handler->OpenFile(absolutePath);
    _builder
        ->SetupResponse()
        .WithStatusCode(HttpStatusCode::_OK)
        .WithLastModified(absolutePath)
        .WithContentType(Utils::GetFileExtension(absolutePath))
        .WithFileDescriptor(fd);
    std::cout << _logger->Log(&Logger::LogInformation, "Response By File Descriptor: ", HttpStatusCode::_OK);
    return (HttpStatusCode::_OK);
}

/**!
 * 
 * POST Assets
 * Use in Post for produces they rules.
 * 
 */

HttpStatusCode::Code Route::Post(HttpRequest &request, std::string absPath) {
    HttpStatusCode::Code result = HttpStatusCode::_DO_NOTHING;
    if ((result = this->_checkBodyLimit(request.GetBodySize()))) { return result; }
    if (this->_handler->PathExist(absPath)) {
        bool isDirectory = this->_handler->FileIsDirectory(absPath);
        bool allow = this->_handler->IsAllowToGetFile(absPath);
        if (allow && isDirectory) {
            if ((result = this->_checkDirectory(absPath, request))) { return result; }
            if (( result = this->_checkExistIndex(request.GetPath(), absPath) )) { return result; }
        }
        else if (allow && Utils::GetFileExtension(absPath) == ".py") {
            if (request._payload.find("Expect:") != request._payload.end()) {
                _builder
                    ->SetupResponse()
                    .WithContentType(".txt")
                    .WithStatusCode(HttpStatusCode::_CONTINUE);
                return HttpStatusCode::_CONTINUE; 
            }
            return this->cgiAction(request, absPath);
        }
        else if (allow) {
            return this->_errorHandler(HttpStatusCode::_BAD_REQUEST);
        }
        else if (!allow) {
            return this->_errorHandler(HttpStatusCode::_FORBIDDEN);
        }
    }
    return _errorHandler(HttpStatusCode::_NOT_FOUND);
}

/**!
 * 
 * DELETE Assets
 * Use in Post for produces they rules.
 * 
 */

bool	Route::removeDirectory(std::string dirPath)
{
	DIR*	dir = opendir(dirPath.c_str());
	struct dirent*	entry;
	while ((entry = readdir(dir)) != NULL)
	{
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
		std::string	fullPath = dirPath + "/" + entry->d_name;
		if (entry->d_type == DT_DIR) {
            if (!removeDirectory(fullPath)) {
                closedir(dir);
                return false;
            }
        } else {
            if (remove(fullPath.c_str()) != 0) {
                closedir(dir);
                return false;
            }
        }
	}
	closedir(dir);
    if (remove(dirPath.c_str()) != 0) {
        return false;
    }
	return true;
}

HttpStatusCode::Code Route::Delete(HttpRequest &request, std::string absPath) {
    HttpStatusCode::Code result = HttpStatusCode::_DO_NOTHING;
    if ((result = this->_checkBodyLimit(request.GetBodySize()))) { return result; }
    if (this->_handler->PathExist(absPath)) {
        bool isDirectory = this->_handler->FileIsDirectory(absPath);
        bool allow = this->_handler->IsAllowToDeleteFile(absPath);

        if (allow && isDirectory) {
			if (removeDirectory(absPath))
            {
                _builder
                    ->SetupResponse()
                    .WithStatusCode(HttpStatusCode::_OK)
                    .WithDefaultPage();
				return HttpStatusCode::_OK;
            }
			return this->_errorHandler(HttpStatusCode::_INTERNAL_SERVER_ERROR);
        }
        else if (allow) {
            if (remove(absPath.c_str()) == 0)
            {
                _builder
                    ->SetupResponse()
                    .WithStatusCode(HttpStatusCode::_OK)
                    .WithDefaultPage();
                return HttpStatusCode::_OK;
            }
            return this->_errorHandler(HttpStatusCode::_INTERNAL_SERVER_ERROR);
        }
        else if (!allow) {
            return this->_errorHandler(HttpStatusCode::_FORBIDDEN);
        }
    }
    return _errorHandler(HttpStatusCode::_NOT_FOUND);
}

HttpStatusCode::Code Route::_errorHandler(HttpStatusCode::Code code) {
    HttpStatusCode::Code result = HttpStatusCode::_DO_NOTHING;
    if ((result = this->_errorHandlerWithFile(code))) { return result; }
    return this->_errorHandlerDefault(code);
}


/**!
 * 
 * GET Assets
 * Use in Post for produces they rules.
 * 
 */

HttpStatusCode::Code Route::cgiAction(HttpRequest &req, std::string absPath) {
    req.client->CreatePair();

    char **envp = req.GetEnvp(Utils::SanitizePath(_root, _upload_on));
    const char *phpInterpreter = "/usr/bin/python3";
    const char *scriptPath = absPath.c_str();
    const char *argv[] = {phpInterpreter, scriptPath, NULL};

    req.client->SetPid(fork());
    if (req.client->GetPid() == 0) {  // Processo filho
        if (chdir(absPath.substr(0, absPath.find_last_of('/')).c_str()) != 0) {
            Utils::DeleteEnvp(envp);
            close(req.client->GetRDPipe());
            close(req.client->GetWRPipe());
            close(req.client->GetWRPipe2());
            close(req.client->GetRDPipe2());
            exit(EXIT_FAILURE);
        }

        close(req.client->GetWRPipe());
        dup2(req.client->GetRDPipe(), STDIN_FILENO);
        dup2(req.client->GetRDPipe2(), STDOUT_FILENO);
        dup2(req.client->GetRDPipe2(), STDERR_FILENO);
 
        close(req.client->GetRDPipe());
        close(req.client->GetRDPipe2());

        execve(phpInterpreter, (char**)argv, envp);
        Utils::DeleteEnvp(envp);
        perror("execve falhou");
        exit(EXIT_FAILURE);
    }
    Utils::DeleteEnvp(envp);
    close(req.client->GetRDPipe2());
    close(req.client->GetRDPipe());
    return HttpStatusCode::_CGI;
}

HttpStatusCode::Code Route::Get(HttpRequest &request, std::string absPath) {
    HttpStatusCode::Code result = HttpStatusCode::_DO_NOTHING;
    if ( request.GetBodySize() > 0 ) { return HttpStatusCode::_BAD_REQUEST; }
    if (this->_handler->PathExist(absPath)) {
        bool isDirectory = this->_handler->FileIsDirectory(absPath);
        bool allow = this->_handler->IsAllowToGetFile(absPath);
        if (allow && isDirectory) {
            if ((result = this->_checkDirectory(absPath, request))) { return result; }
            if (( result = this->_checkExistIndex(request.GetPath(), absPath) )) { return result; }
            if (( result = this->_checkAutoIndex(absPath) )) { return result; }
        }
        else if (allow && Utils::GetFileExtension(absPath) == ".py") {
            return this->cgiAction(request, absPath);
        }
        else if (allow && (result = this->_checkActionInFile(absPath)) ) { return result; }
        else if (!allow) {
            if ((result = this->_errorHandlerWithFile(HttpStatusCode::_FORBIDDEN))) { return result; }
            return this->_errorHandlerDefault(HttpStatusCode::_FORBIDDEN);
        }
    }
    return _errorHandler(HttpStatusCode::_NOT_FOUND);
}

HttpStatusCode::Code Route::_checkExistIndex(std::string reqPath, std::string absPath) {
    std::vector<std::string>::iterator it = this->_indexes.begin();
    for ( ; it != this->_indexes.end(); ++it)
    {
        std::string pathAutoindex = absPath;
        pathAutoindex = Utils::SanitizePath(pathAutoindex, *it);
        if (this->_handler->PathExist(pathAutoindex)
            && this->_handler->IsAllowToGetFile(pathAutoindex)
            && !this->_handler->FileIsDirectory(pathAutoindex)
        )
        {
            _builder
                ->SetupResponse()
                .WithStatusCode(HttpStatusCode::_TEMPORARY_REDIRECT)
                .WithLocation(Utils::SanitizePath("",
                    Utils::SanitizePath(reqPath, *it)))
                .WithDefaultPage();
            std::cout << _logger->Log(&Logger::LogInformation, "Redirect by index: ", HttpStatusCode::_TEMPORARY_REDIRECT);
            return (HttpStatusCode::_TEMPORARY_REDIRECT);
        }
    }
    return HttpStatusCode::_DO_NOTHING;
}

/**!
 * 
 * Seters
 * 
 */

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

void    Route::SetUploadOn(std::string route) {
    this->_upload_on = route;
}

/**!
 * 
 * Geters
 * 
 */

std::string Route::GetRouteName(void) {
    return this->_route_name;
}

size_t Route::GetBodyLimit(void) {
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

std::string Route::GetUploadOn(void) {
    return this->_upload_on;
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

Route::Route(
    ILogger *logger, 
    IServer *server, 
    IHandler *handler, 
    std::string route_name) : 
    _route_name(route_name),
    _allow_methods(server->GetAllowMethods()),
    _error_page(server->GetErrorPages()),
    _limit_client_body_size(server->GetBodyLimit()),
    _redirectPath(server->GetRedirectPath(route_name)),
    _root(server->GetRootDirectory()),
    _upload_on(route_name),
    _autoIndex(server->GetAutoIndex()),
    _indexes(server->GetPageIndexes()),
    _logger(logger),
    _handler(handler)
{
    _builder = new BuilderResponse(_logger, _handler);
    if (_logger->Env()) {
        std::cerr << _logger->Log(&Logger::LogDebug, "Created Route Class: ");
        std::cerr << _logger->Log(&Logger::LogTrace, "Route Standard Content {\n", this->_toString(), "\n}");
    }
}

Route::~Route(void) {
    if (_builder){
        delete _builder;
        _builder = NULL;
    }
    if (_logger != NULL)
        std::cerr << _logger->Log(&Logger::LogDebug, "Deleted Route Class.");
}
