# include "Server.hpp"

Server::Server(IHandler *handler, ILogger *logger) :
_port("8080"),
_limit_client_body_size(2048),
_root("./"),
_autoIndex(false),
_ip("127.0.0.1"),
_ipVersion("IPV4"),
_handler(handler),
_logger(logger),
_actualClientFD(-1)
{
    _hosts.push_back("localhost");
    _allowMethods.insert("GET");
    _handler = handler;
    memset(&hints, 0, sizeof(struct addrinfo));
    this->hints.ai_family = AF_UNSPEC;
    this->hints.ai_socktype = SOCK_STREAM;
    this->hints.ai_flags = AI_CANONNAME;

    this->result = NULL;
    if (_logger->Env()) {
        std::cerr << _logger->Log(&Logger::LogDebug, "Created Server Class: ");
        std::cerr << _logger->Log(&Logger::LogTrace, "Server Standard Content {\n", this->_toString(), "\n}");
    }
}

Server::~Server(void) {
    if (this->result != NULL) {
        freeaddrinfo(this->result);
    }
    std::cerr << _logger->Log(&Logger::LogDebug, "Deleted Server Class.");
}

/* Server Methods
=======================================*/

void    Server::SetAddrInfo(std::string host) {
    int status = 0;

    status = getaddrinfo(
        host.c_str(),
        _port.c_str(),
        &this->hints,
        &this->result
    );
    if (status != 0) {
        throw (Except("Configuration File Not Found!" )); //gai_strerror(status)
    }
}

void    Server::CreateSocketAndBind(void) {
    struct addrinfo *result = this->result;
    int optval = 1;
    int listener = 1;

    for (; result != NULL; result = result->ai_next) {
        listener = socket(
            result->ai_family,
            result->ai_socktype,
            result->ai_protocol
        );
        if (listener == - 1)
            continue;
        if (setsockopt(listener, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof (int)) == -1)
            throw Except("setsockopt");
        if (bind(listener, result->ai_addr, result->ai_addrlen) == 0) {
            break;
        }
        if (close(listener) == -1)
            throw Except("Error on close socket.");
    }
    this->listener = listener;
    if (result == NULL) {
        throw std::invalid_argument(_logger->Log(&Logger::LogInformation, "test"));
    }
    this->_setServerIpInfos(result);
}

int    Server::StartListen(std::string host) {
    std::cout << _logger->Log(&Logger::LogInformation, "Try Starting Listen...");
    if ((listen(this->listener, this->backlog)) == -1) {
        throw Except(_logger->Log(&Logger::LogCaution, "Error on listen server: <", _hosts[0], "> port:  <", _port ,  ">\n"));
    }
    std::cout << _logger->Log(&Logger::LogInformation, "Listening on:", host, _port);
    return this->listener;
}

int Server::AcceptClientConnect(void) {
    socklen_t                   addrlen;
    struct sockaddr_storage     client_saddr;

    addrlen = sizeof(struct sockaddr_storage);
    this->_actualClientFD = accept(
        this->GetListener(),
        (struct sockaddr *) &client_saddr,
        &addrlen
    );
    if (this->_actualClientFD == -1) {
        throw Except("Error on accept client");
    }
    _logger->Log(&Logger::LogInformation, "Client [", this->_actualClientFD, "] connect on");
    return this->_actualClientFD;
}

/* Server Process
=======================================*/

std::string         Server::FindMatchRoute(HttpRequest &res) {
    std::string keyPath = "";
    std::map<std::string, IRoute *>::iterator it = this->_routes.begin();

    for (; it != this->_routes.end(); ++it) {
        int size = it->first.size();
        std::string comp = it->first;
        if (comp[size - 1] != '/' && size++)
            comp += "/";
        std::string subPath = res.GetPath().substr(0, size);
        if (subPath[size - 1] != '/')
            continue;
        if (comp == subPath)
            keyPath = it->first;
    }
    if (keyPath == "")
        keyPath = "/";
    return keyPath;
}

void                Server::ProcessRequest(HttpRequest &request, int client_fd) {
    BuilderResponse builder = BuilderResponse(_logger, _handler);
    std::string keyPath = this->FindMatchRoute(request);
    std::cout << _logger->Log(&ILogger::LogInformation, "Request", "Route", keyPath, request.GetMethod(), request.GetPath());
    std::map<std::string, IRoute *>::iterator it = this->_routes.find(keyPath);
    
    if (it != this->_routes.end()) {
        this->ResponsesMap[client_fd] = this->_routes[keyPath]->ProcessRequest(request);
    } else {
        this->ResponsesMap[client_fd] = builder
            .SetupResponse()
            .WithStatusCode(HttpStatusCode::_INTERNAL_SERVER_ERROR)
            .WithContentType(".html")
            .WithDefaultPage()
            .GetResult();
        std::cout << _logger->Log(&Logger::LogInformation, "Route Not Found or Configurated", HttpStatusCode::_INTERNAL_SERVER_ERROR);
    }
}

IHttpResponse         *Server::ProcessResponse(int client_fd) {
    std::cout << "TAMO AQUI 0 ?" << std::endl;
    IHttpResponse *response = this->ResponsesMap[client_fd];
    if (response == NULL)
        std::cout << "TAMO AQUI 1 ?" << std::endl;
    std::cout << _logger->Log(&ILogger::LogInformation, "Response", response->GetStatusCode(), response->GetStatusMessage());
    std::cout << "TAMO AQUI 2 ?" << std::endl;
    this->ResponsesMap.erase(client_fd);
    std::cout << "TAMO AQUI 3 ?" << std::endl;
    return response;
}

/* Geters
=======================================*/
int   Server::GetListener(void) const {
    return this->listener;
}

std::string Server::GetHosts(void) const {
    std::stringstream   hosts;
    int                 size = this->_hosts.size();

    for (int i = 0; i < size; i++) {
        hosts << this->_hosts[i] << " ";
    }
    return hosts.str();
}

std::string Server::GetIP(void) const {
    return this->_ip;
}

std::string Server::GetIPVersion(void) const {
    return this->_ipVersion;
}

std::string Server::GetListenPort(void) const {
    return _port;
}

/* Seters
=======================================*/
void    Server::_setServerIpInfos(struct addrinfo *result) {
    void    *addr = NULL;
    char ipStr[INET6_ADDRSTRLEN];

    if (result->ai_family == AF_INET) {
        this->_ipVersion = "IPV4";
        sockaddr_in *ipv4= reinterpret_cast<sockaddr_in *>(result->ai_addr);
        addr = &(ipv4->sin_addr);
    }
    else if (result->ai_family == AF_INET6) {
        this->_ipVersion = "IPV6";
        sockaddr_in6 *ipv6= reinterpret_cast<sockaddr_in6 *>(result->ai_addr);
        addr = &(ipv6->sin6_addr);
    }
    inet_ntop(result->ai_family, addr, ipStr, sizeof(ipStr));
    this->_ip = ipStr;
}

// Geters
std::set<std::string> Server::GetAllowMethods(void) {
    return this->_allowMethods;        
}

std::map<HttpStatusCode::Code, std::string> Server::GetErrorPages(void) {
    return this->_errorPages;
}

int Server::GetBodyLimit(void) {
    return this->_limit_client_body_size;
}

std::string Server::GetRedirectPath(std::string path){
    std::map<std::string, std::string>::iterator it = this->_redirectionPaths.find(path);
    if (it != this->_redirectionPaths.end())
        return it->second;
    return "";
}

std::string Server::GetRootDirectory(void) {
    return this->_root;
}

std::vector<std::string>    Server::GetPageIndexes(void) {
    return this->_indexes;
}

bool    Server::GetAutoIndex(void) {
    return this->_autoIndex;
}

std::vector<std::string>    Server::GetHosts(void) {
    return this->_hosts;
}

std::string Server::GetPort(void) {
    return this->_port;
}

IRoute  *Server::GetRoute(std::string routeName) {
    std::map<std::string, IRoute *>::iterator it = this->_routes.find(routeName);
    if (it != this->_routes.end())
        return it->second;
    return NULL;
}

// Seters
void    Server::SetAllowMethods(std::set<std::string> methods) {
    this->_allowMethods.clear();
    std::set<std::string>::iterator it = methods.begin();
    for ( ; it != methods.end(); ++it) {
        this->_allowMethods.insert(*it);
    }
}

void    Server::SetErrorPage(std::set<HttpStatusCode::Code> statusCodes, std::string filePath) {
    std::set<HttpStatusCode::Code>::iterator it = statusCodes.begin();
    for ( ; it != statusCodes.end(); ++it) {
        this->_errorPages[*it] = filePath;
    }
}

void    Server::SetBodyLimit(int size) {
    this->_limit_client_body_size = size;
}

void    Server::SetRedirectPath(std::pair<std::string, std::string> pair) {
    this->_redirectionPaths[pair.first] = pair.second;
}

void    Server::SetRootDirectory(std::string root) {
    this->_root = root;
}
void    Server::SetPagesIndexes(std::vector<std::string> indexes) {
    this->_indexes.clear();
    std::vector<std::string>::iterator it = indexes.begin();
    for ( ; it != indexes.end(); ++it) {
        this->_indexes.push_back(*it);
    }
}

void    Server::SetAutoIndex(bool flag) {
    this->_autoIndex = flag;
}

void    Server::SetHosts(std::vector<std::string> hosts) {
    this->_hosts.clear();
    std::vector<std::string>::iterator it = hosts.begin();
    for ( ; it != hosts.end(); ++it) {
        this->_hosts.push_back(*it);
    }
}

void    Server::SetPort(std::string port) {
    this->_port = port;
}

void    Server::SetRoute(std::string routeName, IRoute *route) {
    this->_routes[routeName] = route;
}

std::string Server::_toString(void) {
    std::stringstream ss;

    ss << "\t\tClient Connected: " << _actualClientFD << std::endl;
    ss << "\t\tServer Names: ";
    for (std::vector<std::string>::iterator it = _hosts.begin(); it != _hosts.end(); ++it) {
        ss << *it << " ";
    }
    ss << std::endl;
    ss << "\t\tListening on: " << _port << std::endl;
    ss << "\t\tAllow Methods: ";
    for (std::set<std::string>::iterator it = _allowMethods.begin() ; it != _allowMethods.end(); ++it) {
        ss << *it << " ";
    }
    ss << std::endl << "\t\tError Pages: " << std::endl;
    for (std::map<HttpStatusCode::Code, std::string>::iterator it = _errorPages.begin(); it != _errorPages.end(); ++it) {
        ss << "\t\t\t" << static_cast<int>(it->first) << " " << it->second << std::endl;
    }
    ss << "\t\tBody Limit: " << _limit_client_body_size << std::endl;
    ss << "\t\tRedirect Path: " << std::endl;
    for (std::map<std::string, std::string>::iterator it = _redirectionPaths.begin(); it != _redirectionPaths.end(); ++it) {
        ss << "\t\t\t" << it->first << " " << it->second << std::endl;
    }
    ss << "\t\tRoot Directory: " << _root  << std::endl;
    ss << "\t\tAuto index: " << std::string((_autoIndex) ? "on" : "off") << std::endl;
    ss << "\t\tindexes: ";
    for (std::vector<std::string>::iterator it = _indexes.begin(); it != _indexes.end(); ++it) {
        ss << *it << " ";
    }
    ss << std::endl;
    ss << "\t\tRoutes: " << std::endl;
    for (std::map<std::string, IRoute *>::iterator it = _routes.begin(); it != _routes.end(); ++it) {
        ss << "\t\t\t" << it->first << " " << std::string(it->second != NULL ? "has" : "empty")  << std::endl;
    }
    return ss.str();
}