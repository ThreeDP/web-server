# include "Server.hpp"

/* Server Methods
=======================================*/
void    Server::CreateSocketAndBind(void) {
    struct addrinfo *result = this->result;
    int optval = 1;
    int listener = 1;

    unsigned int numOfAddr = 0;
        char ipStr[INET6_ADDRSTRLEN];
    for (; result != NULL; result = result->ai_next) {
        void*addr;
        std::string ipVer;

        if (result->ai_family == AF_INET) {
            ipVer = "IPV4";
            sockaddr_in *ipv4= reinterpret_cast<sockaddr_in *>(result->ai_addr);
            addr = &(ipv4->sin_addr);
            numOfAddr++;
        }
        else if (result->ai_family == AF_INET6) {
            ipVer = "IPV6";
            sockaddr_in6 *ipv6= reinterpret_cast<sockaddr_in6 *>(result->ai_addr);
            addr = &(ipv6->sin6_addr);
            numOfAddr++;
        }
		inet_ntop(result->ai_family, addr, ipStr, sizeof(ipStr));
		std::cout << "( " << numOfAddr << " ) " << ipVer << " : " << ipStr << std::endl;

        listener = socket(
            result->ai_family,
            result->ai_socktype,
            result->ai_protocol);
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
}

int    Server::StartListen(void) {
    if ((listen(this->listener, this->backlog)) == -1) {
        throw Except("Error on listen server: <name> port: <port>");
    }
    std::cout << "Start Listen!" << std::endl;
    return this->listener;
}

/* Geters
=======================================*/
int   Server::GetListener(void) const {
    return this->listener;
}

/* Seters
=======================================*/
void    Server::SetAddrInfo(void) {
    int status = 0;
    std::stringstream port;

    port << this->listen_port;
    status = getaddrinfo(NULL, "8000", &this->hints, &this->result);
    if (status != 0) {
        throw (Except("Configuration File Not Found!" )); //gai_strerror(status)
    }
}

/* Base Methods
=======================================*/
Server::Server(void) {
    memset(&hints, 0, sizeof(struct addrinfo));
    this->hints.ai_family = AF_UNSPEC;
    this->hints.ai_socktype = SOCK_STREAM;
    this->hints.ai_flags = AI_CANONNAME;

    this->result = NULL;
}

Server::~Server(void) {
    if (this->result != NULL) {
        freeaddrinfo(this->result);
    }
}