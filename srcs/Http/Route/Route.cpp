# include "Route.hpp"

/**!
 * 
 * Init Method:
 * Route processes a request
 * 
 */

// IHttpResponse *Route::ProcessRequest(
//     HttpRequest &request
// ) {
//     std::string     absolutePath;
    
//     absolutePath = Utils::SanitizePath(this->_root, request.GetPath());
//     if (this->_checkAllowMethod(request.GetMethod())) {
//         return _builder->GetResult(); 
//     }
//     if (this->_checkRedirectPath(this->GetRedirectPath())) {
//         return _builder->GetResult();
//     }
//     if (((this->*_httpMethods[request.GetMethod()])( request, absolutePath ) )) {
//         return _builder->GetResult();
//     }
//     _builder
//         ->SetupResponse()
//         .WithStatusCode(HttpStatusCode::_INTERNAL_SERVER_ERROR)
//         .WithContentType(".html")
//         .WithDefaultPage()
//         .GetResult();
//     std::cout << _logger->Log(&Logger::LogInformation, "Route Not Found or Configurated", HttpStatusCode::_INTERNAL_SERVER_ERROR);
//     return _builder->GetResult();
// }

IHttpResponse *Route::ProcessRequest(
    HttpRequest &request,
    int* cgifd,
    int epoll
) {
    std::string     absolutePath;
    
    absolutePath = Utils::SanitizePath(this->_root, request.GetPath());
    if (this->_checkAllowMethod(request.GetMethod())) {
        return _builder->GetResult(); 
    }
    if (this->_checkRedirectPath(this->GetRedirectPath())) {
        return _builder->GetResult();
    }

    if (request.GetMethod() == "GET") {
        if (this->Get( request, absolutePath, cgifd, epoll ) == HttpStatusCode::_CGI) {
            return NULL;
        }
        return _builder->GetResult();
    } else if (request.GetMethod() == "POST") {
        if (this->Post( request, absolutePath, cgifd, epoll ) == HttpStatusCode::_CGI) {
            return NULL;
        }
        return _builder->GetResult();
    } else if (request.GetMethod() == "DELETE") {
        if (this->Delete( request, absolutePath, cgifd, epoll ) == HttpStatusCode::_CGI) {
            return NULL;
        }
        return _builder->GetResult();
    }

    _builder->SetupResponse()
        .WithStatusCode(HttpStatusCode::_INTERNAL_SERVER_ERROR)
        .WithContentType(".html")
        .WithDefaultPage()
        .GetResult();
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
    if (!this->IsAllowMethod(method)) {
        HttpStatusCode::Code status = HttpStatusCode::_METHOD_NOT_ALLOWED;
        HttpStatusCode::Code result = HttpStatusCode::_DO_NOTHING;

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

HttpStatusCode::Code Route::_notFound(void) {
    HttpStatusCode::Code status = HttpStatusCode::_NOT_FOUND;
    HttpStatusCode::Code result = HttpStatusCode::_DO_NOTHING;
    
    if ((result = this->_errorHandlerWithFile(status))) { return result; }
    return this->_errorHandlerDefault(status);
}

/**!
 * 
 * POST Assets
 * Use in Post for produces they rules.
 * 
 */

HttpStatusCode::Code Route::Post(HttpRequest &request, std::string absPath, int* cgifd, int epoll) {
    HttpStatusCode::Code result = HttpStatusCode::_DO_NOTHING;
    if ((result = this->_checkBodyLimit(request.GetBodySize()))) { return result; }
    if (this->_handler->PathExist(absPath)) {
        bool isDirectory = this->_handler->FileIsDirectory(absPath);
        bool allow = this->_handler->IsAllowToGetFile(absPath);
        if (allow && isDirectory) {
            if (( result = this->_checkExistIndex(request.GetPath(), absPath) )) { return result; }
            if (( result = this->_checkAutoIndex(absPath) )) { return result; }
        }
        else if (allow && Utils::GetFileExtension(absPath) == ".py") {
            this->cgiAction(request, epoll, absPath, cgifd);
            return HttpStatusCode::_CGI;
        }
        else if (allow){
            if ((result = this->_errorHandlerWithFile(HttpStatusCode::_BAD_REQUEST))) { return result; }
            return this->_errorHandlerDefault(HttpStatusCode::_BAD_REQUEST);
        }
        else if (!allow) {
            if ((result = this->_errorHandlerWithFile(HttpStatusCode::_FORBIDDEN))) { return result; }
            return this->_errorHandlerDefault(HttpStatusCode::_FORBIDDEN);
        }
    }
    return this->_notFound();
}


/**!
 * 
 * DELETE Assets
 * Use in Post for produces they rules.
 * 
 */

HttpStatusCode::Code Route::Delete(HttpRequest &request, std::string absPath, int* cgifd, int epoll) {
    HttpStatusCode::Code result = HttpStatusCode::_DO_NOTHING;
    if ((result = this->_checkBodyLimit(request.GetBodySize()))) { return result; }
    if (this->_handler->PathExist(absPath)) {
        bool isDirectory = this->_handler->FileIsDirectory(absPath);
        bool allow = this->_handler->IsAllowToGetFile(absPath);
        if (allow && isDirectory) {
            if (( result = this->_checkExistIndex(request.GetPath(), absPath) )) { return result; }
            if (( result = this->_checkAutoIndex(absPath) )) { return result; }
        }
        else if (allow && Utils::GetFileExtension(absPath) == ".py") {
            this->cgiAction(request, epoll, absPath, cgifd);
            return HttpStatusCode::_CGI;
        }
        else if (allow){
            if ((result = this->_errorHandlerWithFile(HttpStatusCode::_BAD_REQUEST))) { return result; }
            return this->_errorHandlerDefault(HttpStatusCode::_BAD_REQUEST);
        }
        else if (!allow) {
            if ((result = this->_errorHandlerWithFile(HttpStatusCode::_FORBIDDEN))) { return result; }
            return this->_errorHandlerDefault(HttpStatusCode::_FORBIDDEN);
        }
    }
    return this->_notFound();
}


/**!
 * 
 * GET Assets
 * Use in Post for produces they rules.
 * 
 */

void Route::cgiAction(HttpRequest &req, int epollFD, std::string absPath, int* cgifd) {
    // Deve adicionar o GetEnvp no envp do server em todo request.
	std::vector<std::string> ev = req.GetEnvp();
	char **envp = new char*[ev.size() + 1];

	for (size_t i = 0; i < ev.size(); ++i) {
		envp[i] = new char[ev[i].size() + 1];  
		std::strcpy(envp[i], ev[i].c_str());
	}
    

	envp[ev.size()] = NULL;

	const char *phpInterpreter = "/usr/bin/python3";
    const char *scriptPath = absPath.c_str();
	const char *argv[] = {phpInterpreter, scriptPath, NULL};

	pid_t pid = fork();
	if (pid == 0) {
        close(cgifd[0]);

        dup2(cgifd[1], STDOUT_FILENO);
        dup2(cgifd[1], STDERR_FILENO);

        close(cgifd[1]);

		execve(phpInterpreter, (char**)argv, envp);
        exit(1);
	} else {
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            perror("Erro ao esperar pelo processo filho com waitpid");
            // Trate o erro conforme necessário, talvez com uma saída antecipada ou tentativa de recuperação.
            exit(EXIT_FAILURE);
        }

        // Verifica se o processo filho terminou normalmente.
        if (WIFEXITED(status)) {
            int exitStatus = WEXITSTATUS(status);
            if (exitStatus != 0) {
                std::cerr << "O processo filho terminou com status de erro: " << exitStatus << std::endl;
                // Trate o erro conforme necessário.
            }
        } else if (WIFSIGNALED(status)) {
            int signal = WTERMSIG(status);
            std::cerr << "O processo filho foi terminado por um sinal: " << signal << std::endl;
            // Trate o erro conforme necessário.
        } else {
            std::cerr << "O processo filho terminou de maneira inesperada." << std::endl;
            // Trate o erro conforme necessário.
        }

        if (close(cgifd[1]) == -1) {
            perror("Erro ao fechar o socket no processo pai");
            // Trate o erro conforme necessário.
            exit(EXIT_FAILURE);
        }

        struct epoll_event event;
        memset(&event, 0, sizeof(struct epoll_event));
        event.events = EPOLLIN;
        event.data.fd = cgifd[0];

        if (epoll_ctl(epollFD, EPOLL_CTL_ADD, cgifd[0], &event) == -1) {
            perror("Erro ao adicionar o descritor ao epoll");
            // Trate o erro conforme necessário, como fechando o socket e/ou liberando recursos.
            exit(EXIT_FAILURE);
        }
	}
}

HttpStatusCode::Code Route::Get(HttpRequest &request, std::string absPath, int* cgifd, int epoll) {
    HttpStatusCode::Code result = HttpStatusCode::_DO_NOTHING;
    std::cout << _logger->Log(&Logger::LogCaution, absPath) << std::endl;
    if ( request.GetBodySize() > 0 ) { return HttpStatusCode::_BAD_REQUEST; }
    if (this->_handler->PathExist(absPath)) {
        bool isDirectory = this->_handler->FileIsDirectory(absPath);
        bool allow = this->_handler->IsAllowToGetFile(absPath);
        if (allow && isDirectory) {
            if (( absPath[absPath.length() - 1] != '/')) {
                _builder->SetupResponse()
                    .WithContentType(".html")
                    .WithStatusCode(HttpStatusCode::_TEMPORARY_REDIRECT)
                    .WithLocation(Utils::SanitizePath("", absPath + "/"))
                    .WithDefaultPage();
                    return HttpStatusCode::_TEMPORARY_REDIRECT;
            }
            if (( result = this->_checkExistIndex(request.GetPath(), absPath) )) { return result; }
            if (( result = this->_checkAutoIndex(absPath) )) { return result; }
        }
        else if (allow && Utils::GetFileExtension(absPath) == ".py") {
            this->cgiAction(request, epoll, absPath, cgifd);
            return HttpStatusCode::_CGI;
        }
        else if (allow && (result = this->_checkActionInFile(absPath)) ) { return result; }
        else if (!allow) {
            if ((result = this->_errorHandlerWithFile(HttpStatusCode::_FORBIDDEN))) { return result; }
            return this->_errorHandlerDefault(HttpStatusCode::_FORBIDDEN);
        }
    }
    return this->_notFound();
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
    _autoIndex(server->GetAutoIndex()),
    _indexes(server->GetPageIndexes()),
    _logger(logger),
    _handler(handler)
{
    // _httpMethods["GET"] = &Route::Get;
    // _httpMethods["POST"] = &Route::Post;
    // _httpMethods["DELETE"] = &Route::Delete;
    _builder = new BuilderResponse(_logger, _handler);
    if (_logger->Env()) {
        std::cerr << _logger->Log(&Logger::LogDebug, "Created Route Class: ");
        std::cerr << _logger->Log(&Logger::LogTrace, "Route Standard Content {\n", this->_toString(), "\n}");
    }
}

Route::~Route(void) {
    std::cerr << _logger->Log(&Logger::LogDebug, "Deleted Route Class.");
}
