# include "Server.hpp"

/* Server Methods
=======================================*/

void    Server::SetAddrInfo(void) {
    int status = 0;
    std::stringstream port;

    port << this->listen_port;
    status = getaddrinfo(
        this->server_names[0].c_str(),
        port.str().c_str(),
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
    std::cout << *this;
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

std::string         Server::ProcessResponse(int client_fd) {
    std::string response = this->ClientsResponse[client_fd].CreateResponse();
    this->ClientsResponse.erase(client_fd);
    this->UpdateState(S_SERVER_RESPONSE, client_fd);
    return response;
}

void                Server::ProcessRequest(std::string buffer, int client_fd) {
    HttpRequest     res;
    DIR             *dir = NULL;
    res.ParserRequest(buffer);
    std::string     path = std::string("." + res.GetPath());

    dir = opendir(path.c_str());
    if (dir != NULL) {
        struct dirent* entry;

        while ((entry = readdir(dir)) != NULL) {
            std::string d_name = entry->d_name; 
            if (d_name == "index.html") {
                path += "/" + d_name;
            }
        }
        closedir(dir);
    }
    std::string body;
    std::ifstream file(path.c_str());
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            body += line;
        }
        file.close();
    }
    this->ClientsResponse[client_fd] = HttpResponse(
        body,
        "200",
        "text/html"
    );
    this->UpdateState(S_CLIENT_REQUEST, client_fd);
}

/* Geters
=======================================*/
int   Server::GetListener(void) const {
    return this->listener;
}

std::string Server::GetHosts(void) const {
    std::stringstream   hosts;
    int                 size = this->server_names.size();

    for (int i = 0; i < size; i++) {
        hosts << this->server_names[i] << " ";
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

    ls_port << this->listen_port;
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
Server::Server(void) {
    memset(&hints, 0, sizeof(struct addrinfo));
    this->hints.ai_family = AF_UNSPEC;
    this->hints.ai_socktype = SOCK_STREAM;
    this->hints.ai_flags = AI_CANONNAME;

    this->result = NULL;
    this->_stage = S_START;
    std::cout << *this;
}

Server::Server(std::vector<std::string> serv, unsigned short port) : server_names(serv), listen_port(port) {
    memset(&hints, 0, sizeof(struct addrinfo));
    this->hints.ai_family = AF_UNSPEC;
    this->hints.ai_socktype = SOCK_STREAM;
    this->hints.ai_flags = AI_PASSIVE;

    this->result = NULL;
    this->_stage = S_START;
    std::cout << *this;
}

Server::~Server(void) {
    if (this->result != NULL) {
        freeaddrinfo(this->result);
    }
}

std::ostream &operator<<(std::ostream &os, Server const &server) {
    switch (server.GetStage()) {
    case S_START:
        os << HBLU "[ Create Server: ]" reset << std::endl;
        break;
    case S_LISTEN:
        os << HBLU "[ Start Server with Hosts: " << server.GetHosts() << 
        "on [" << server.GetIPVersion() << "]: " << server.GetIP() << 
        " port: " << server.GetListenPort() << " ]" reset << std::endl;
        break;
    case S_CLIENT_CONNECT:
        os << HBLU "[ Client " << server.GetClientFD() << " connected to server " << server.GetIP() << ":" << server.GetListenPort() << " ]" reset << std::endl; 
        break;
    case S_CLIENT_DISCONNECT:
        os << HBLU "[ Client " << server.GetClientFD() << " disconnected to server " << server.GetIP() << ":" << server.GetListenPort() << " ]" reset << std::endl; 
        break;
    case S_SERVER_RESPONSE:
        os << HBLU "[ Server " << server.GetIP() << ":" << server.GetListenPort() <<
        " sending response to " << server.GetClientFD() << " ]" reset << std::endl;
        break;
    case S_CLIENT_REQUEST:
        os << HBLU "[ Server " << server.GetIP() << ":" << server.GetListenPort() <<
        " received a request from " << server.GetClientFD() << " ]" reset << std::endl;
        break;
    default:
        os << "Error";
    }
	return (os);
}