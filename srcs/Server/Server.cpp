# include "Server.hpp"

/* Server Methods
=======================================*/

void    Server::SetAddrInfo(void) {
    int status = 0;
    status = getaddrinfo(
        "localhost",
        _port.c_str(),
        &this->hints,
        &this->result
    );
    if (status != 0) {
        throw (Except("Configuration File Not Found!" )); //gai_strerror(status)
    }
    std::cout << "SERVER ADDRINFO" << std::endl;
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
        if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof (int)) == -1)
            throw Except("setsockopt");
        if (bind(listener, result->ai_addr, result->ai_addrlen) == 0) {
            break;
        }
        if (close(listener) == -1)
            throw Except("Error on close socket.");
    }
    this->listener = listener;
    if (result == NULL) {
        throw Except("Not able to bind server: <name> port: <port>.");
    }
    this->_setServerIpInfos(result);
}

int    Server::StartListen(void) {
    if ((listen(this->listener, this->backlog)) == -1) {
        throw Except("Error on listen server: <name> port: <port>");
    }
    // std::cout << *this;
    return this->listener;
}

int Server::AcceptClientConnect(void) {
    socklen_t                   addrlen;
    struct sockaddr_storage     client_saddr;

    this->_stage = S_CLIENT_CONNECT;
    addrlen = sizeof(struct sockaddr_storage);
    this->_actualClientFD = accept(
        this->GetListener(),
        (struct sockaddr *) &client_saddr,
        &addrlen
    );
    if (this->_actualClientFD == -1) {
        throw Except("Error on accept client");
    }
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
    this->UpdateState(S_CLIENT_REQUEST, client_fd);
    BuilderResponse builder = BuilderResponse(new Handler());
    std::string keyPath = this->FindMatchRoute(request);
    this->_routes[keyPath]->ProcessRequest(request, builder);
    this->ResponsesMap[client_fd] = builder.GetResult();
    std::cout << this->ResponsesMap[client_fd]->ToString() << std::endl;
}

std::string         Server::ProcessResponse(int client_fd) {
    std::string res = this->ResponsesMap[client_fd]->ToString();
    this->ResponsesMap.erase(client_fd);
    this->UpdateState(S_SERVER_RESPONSE, client_fd);
    return res;
}

/* Geters
=======================================*/
int   Server::GetListener(void) const {
    return this->listener;
}

std::string Server::GetHosts(void) const {
    std::stringstream   hosts;
    int                 size = this->_server_names.size();

    for (int i = 0; i < size; i++) {
        hosts << this->_server_names[i] << " ";
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
    std::stringstream ls_port;

    ls_port << this->_listen_port;
    return ls_port.str();
}

ServerStages    Server::GetStage(void) const {
    return this->_stage;
}

int             Server::GetClientFD(void) const {
    return this->_actualClientFD;
}

/* Seters
=======================================*/
void    Server::_setServerIpInfos(struct addrinfo *result) {
    void    *addr;
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
    this->_stage = S_LISTEN;
}

void    Server::UpdateState(ServerStages st, int client_fd) {
    this->_stage = st;
    this->_actualClientFD = client_fd;
}

/* Base Methods
=======================================*/
// Server::Server(void) {
//     memset(&hints, 0, sizeof(struct addrinfo));
//     this->hints.ai_family = AF_UNSPEC;
//     this->hints.ai_socktype = SOCK_STREAM;
//     this->hints.ai_flags = AI_CANONNAME;

//     this->result = NULL;
//     this->_stage = S_START;
//     std::cout << *this;
// }

// Server::Server(std::vector<std::string> serv, unsigned short port) : CommonParameters(serv, port) {
//     memset(&hints, 0, sizeof(struct addrinfo));
//     this->hints.ai_family = AF_UNSPEC;
//     this->hints.ai_socktype = SOCK_STREAM;
//     this->hints.ai_flags = AI_PASSIVE;

//     this->result = NULL;
//     this->_stage = S_START;
//     std::cout << *this;
// }

// Server::Server(std::string name) : CommonParameters(){
//     this->_default_error_page[403] = "/errors/403.html";
//     this->_default_error_page[404] = "/404.html";
//     this->_default_error_page[500] = "/50x.html";
//     this->_default_error_page[502] = "/50x.html";
//     this->_default_error_page[503] = "/50x.html";
//     this->_default_error_page[504] = "/50x.html";
//     this->_root = "../home";
//     this->_index.insert("index.html");
//     this->_index.insert("new.html");
//     this->_autoindex = false;
//     this->_stage = S_START;
//     std::cout << *this;
// }

// Server::Server(std::string name, IHandler *handler) : CommonParameters(){
//     this->_default_error_page[403] = "/errors/403.html";
//     this->_default_error_page[404] = "/404.html";
//     this->_default_error_page[500] = "/50x.html";
//     this->_default_error_page[502] = "/50x.html";
//     this->_default_error_page[503] = "/50x.html";
//     this->_default_error_page[504] = "/50x.html";
//     this->_root = "../home";
//     this->_index.insert("index.html");
//     this->_index.insert("new.html");
//     this->_autoindex = false;
//     this->_stage = S_START;
//     this->_handler = handler;
//     std::cout << *this;
// }

// Server(std::string name){
//     this->_listen_host = "127.0.0.1";
//     this->_listen_port = 8081;
//     this->_server_names.push_back(name);
//     // this->_default_error_page[404] = "/404.html";
//     // this->_default_error_page[500] = "/50x.html";
//     // this->_default_error_page[502] = "/50x.html";
//     // this->_default_error_page[503] = "/50x.html";
//     // this->_default_error_page[504] = "/50x.html";
//     this->_limit_client_body_size = 2 * 1024;
//     this->_root = "../home";
//     this->_index.insert("index.html");
//     this->_index.insert("new.html");
//     this->_autoindex = false;
// }
    

// Server::Server(std::string name, int port, std::string root) : CommonParameters(){
//     this->_default_error_page[403] = "/errors/403.html";
//     this->_default_error_page[404] = "/404.html";
//     this->_default_error_page[500] = "/50x.html";
//     this->_default_error_page[502] = "/50x.html";
//     this->_default_error_page[503] = "/50x.html";
//     this->_default_error_page[504] = "/50x.html";
//     this->_listen_port = port;
//     this->_root = root;
//     this->_index.insert("index.html");
//     this->_index.insert("new.html");
//     this->_autoindex = false;
//     this->_stage = S_START;
//     std::cout << *this;
// }

Server::~Server(void) {
    if (this->result != NULL) {
        freeaddrinfo(this->result);
    }
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
    this->_errorPages.clear();
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
