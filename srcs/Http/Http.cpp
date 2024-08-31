# include "Http.hpp"

/* Http Methods
=================================================*/
void    Http::StartPollList(void) {
    struct epoll_event  event;

    int &epollFD = this->GetEPollFD();
    if (epollFD == -1) {
        throw Except("Error on create Epoll");
    }
    std::map<std::string, IServer*>::iterator it = this->servers.begin();
    for (; it != this->servers.end(); ++it) {
        memset(&event, 0, sizeof(struct epoll_event));
        it->second->SetAddrInfo(it->first);
        it->second->CreateSocketAndBind();
        event.data.fd = it->second->StartListen(it->first);
        event.events = EPOLLIN;
        if (epoll_ctl(epollFD, EPOLL_CTL_ADD, it->second->GetListener(), &event) == -1)
            throw Except("Error on add server: <name> port: <port>");
        _clientFDToStringHost[it->second->GetListener()] = it->first;
        _clientFDToServer[it->second->GetListener()] = it->second;
    }
}

void    Http::StartWatchSockets(void) {
    // std::cout << *this;
    while (true) {
        int number_of_ready_fds = epoll_wait(
            this->GetEPollFD(),
            this->clientEvents,
            this->eventsLimit,
            -1
        );
        if (number_of_ready_fds == -1) {
            throw Except("Epoll Wait error!");
        }
        for (int i = 0; i < number_of_ready_fds; i++) {
            if ((this->clientEvents[i].events & EPOLLIN) == EPOLLIN) {
                // Verifica se for feito um handshake e da continuidade pra o proximo fd.
                if (this->ConnectClientToServer(i))
                    continue;
                ssize_t numbytes = this->HandleRequest(this->clientEvents[i].data.fd);  
                if (numbytes == -1)
                    throw Except("error recv");
                if (numbytes == 0) {
                    this->DisconnectClientToServer(this->clientEvents[i].data.fd);
                } else if (numbytes > 0) {
                    this->HandleResponse(this->clientEvents[i].data.fd);
                    this->DisconnectClientToServer(this->clientEvents[i].data.fd);
                    break;
                }
            }
            break;
        }
    }
}

void    Http::DisconnectClientToServer(int client_fd) {
    struct epoll_event  event;
    event.data.fd = client_fd;
    event.events = EPOLLIN;

    if (epoll_ctl (this->GetEPollFD(), EPOLL_CTL_DEL, client_fd, &event) == -1)
        throw Except("epoll_ctl");
    if (close(client_fd) == -1)
        throw Except("close");
    IServer *server = this->clientFD_Server[client_fd];
    this->clientFD_Server.erase(client_fd);

    std::cout << _logger->Log(&ILogger::LogInformation, "Disconnect Client", client_fd, "from server", server->GetIP(), "on port", server->GetPort()) << std::endl;
}

bool    Http::ConnectClientToServer(int i) {
    bool hasHandShake = false;
    std::map<std::string, IServer*>::iterator it = this->servers.begin();
    // Busca o Server compativel com o fd do request.
    for (; it != this->servers.end(); ++it) {
        int fd = this->clientEvents[i].data.fd;
        if (fd == it->second->GetListener()) {
            _logger->Log(&Logger::LogInformation, "New client try to connect on:", _clientFDToStringHost[fd]);
            this->ClientHandShake(it->second);
            hasHandShake = true;
            break;
        }
    }
    return hasHandShake;
}

ssize_t    Http::HandleRequest(int client_fd) {
    char        buffer[1000000];
    HttpRequest res;
    IServer      *server = this->clientFD_Server[client_fd];

    memset(&buffer, 0, sizeof(char) * 1000000);
    ssize_t numbytes = recv(client_fd, &buffer, sizeof(char) * 1000000, 0);
    res.ParserRequest(buffer);
    if (server != NULL) 
        server->ProcessRequest(res, client_fd);
    return numbytes;
}

void    Http::HandleResponse(int client_fd) {
    IServer *server = this->clientFD_Server[client_fd];
    IHttpResponse *message = server->ProcessResponse(client_fd);

    std::vector<char> test = message->CreateResponse();
    if (send(client_fd, &test[0], test.size(), 0) == -1) {
        delete message;
        throw Except("Error on send Response");
    }
    delete message;
}

void    Http::ClientHandShake(IServer *server) {
    int                         client_fd;
    struct epoll_event          event;

    memset(&event, 0, sizeof(event));
    client_fd = server->AcceptClientConnect();
    event.events = EPOLLIN;
    event.data.fd = client_fd;
    if (epoll_ctl(this->GetEPollFD(), EPOLL_CTL_ADD, client_fd, &event) == 1) {
        throw Except("Error on add client on epoll.");
    }
    this->clientFD_Server[client_fd] = server;

    std::cout << _logger->Log(&ILogger::LogInformation, "Connect Client", client_fd, "from server", server->GetIP(), "on port", server->GetPort()) << std::endl;
}

/* Geters
=================================================*/
IServer *Http::GetServer(std::string server) {
    std::map<std::string, IServer *>::iterator it = this->servers.find(server);
    if (it != this->servers.end()) {
        return it->second;
    }
    return NULL;
}

int &Http::GetEPollFD(void) {
    static int epollFD = epoll_create1(0);
    return epollFD;
}

/* Seters
=================================================*/
void Http::SetServer(std::string serverName, IServer *server) {
    this->servers[serverName] = server;
}

void Http::SetServer(IServer *server) {
    std::vector<std::string> hosts = server->GetHosts();
    std::vector<std::string>::iterator it = hosts.begin();
    for ( ; it != hosts.end(); ++it) {
        this->servers[*it] = server;
    }
    _serversPointer.push_back(server);
}

/* Base Methods
=================================================*/
Http::Http(ILogger *logger) {
    HttpResponse::SetDefaultHTTPResponse();
    _logger = logger;
}

Http::~Http(void) {
    std::vector<IServer *>::iterator it = _serversPointer.begin();
    for ( ; it != _serversPointer.end(); ++it) {
        delete *it;
    }
}