# include "Http.hpp"

void modify_epoll_event(int epoll_fd, int sock_fd, uint32_t new_events) {
    // Remove o socket atual do epoll
    struct epoll_event event;
    memset(&event, '\0', sizeof(struct epoll_event));
    if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, sock_fd, NULL) == -1) {
        std::cerr << "Erro ao remover o socket do epoll: " << strerror(errno) << std::endl;
        return;
    }

    // Configura o novo evento
    event.events = new_events;
    event.data.fd = sock_fd;

    // Adiciona o socket de volta ao epoll com o novo evento
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sock_fd, &event) == -1) {
        std::cerr << "Erro ao adicionar o socket ao epoll: " << strerror(errno) << std::endl;
    }
}

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
        _serverFDToStringHost[it->second->GetListener()] = it->first;
        _serverFDToServer[it->second->GetListener()] = it->second;
    }
}

void    Http::StartWatchSockets(void) {
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
        std::cout << _logger->Log(&Logger::LogInformation, "Number of fds ready to read:", number_of_ready_fds);
        for (int i = 0; i < number_of_ready_fds; i++) {
            std::map<int, int>::iterator it = _cgis.find(this->clientEvents[i].data.fd);
            std::cout <<"AQUI 1"<< std::endl;
            if ((this->clientEvents[i].events & EPOLLIN) == EPOLLIN) {
                std::cout <<"AQUI 2"<< std::endl;
                if (it != _cgis.end()) {
                    std::cout <<"AQUI 3"<< std::endl;
                    clientFD_Server[it->second]->CreateCGIResponse(this->GetEPollFD(), it->first, it->second);
                    // modify_epoll_event(this->GetEPollFD(), this->clientEvents[i].data.fd, EPOLLOUT);
                    _cgis.erase(it->first);
                    std::cout <<"AQUI 6"<< std::endl;
                    break;
                } else {
                    if (this->ConnectClientToServer(this->clientEvents[i].data.fd))
                        continue;
                    std::cout <<"AQUI 4"<< std::endl;
                    ssize_t numbytes = this->HandleRequest(this->clientEvents[i].data.fd, this->GetEPollFD());
                    modify_epoll_event(this->GetEPollFD(), this->clientEvents[i].data.fd, EPOLLOUT);
                    if (numbytes == -1)
                        throw Except("error recv");
                    if (numbytes == 0) {
                        this->DisconnectClientToServer(this->clientEvents[i].data.fd);
                    }  
                    if (numbytes > 0){
                        break ;
                    }
                    std::cout <<"AQUI 8"<< std::endl;
                }
            }   
            if ((this->clientEvents[i].events & EPOLLOUT)) {
                std::cout <<"EPOLLOUT IN"<< std::endl;
                this->HandleResponse(this->clientEvents[i].data.fd);
                this->DisconnectClientToServer(this->clientEvents[i].data.fd);
                std::cout <<"EPOLLOUT OUT"<< std::endl;
                break;
            }
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

    std::cout << _logger->Log(&ILogger::LogInformation, "Disconnect Client", client_fd, "from server", server->GetIP(), "on port", server->GetPort());
}

bool    Http::ConnectClientToServer(int fd) {
    bool hasHandShake = false;
    std::map<int, IServer*>::iterator it = this->_serverFDToServer.find(fd);
    if (it != this->_serverFDToServer.end()) {
            _logger->Log(&Logger::LogInformation, "New client try to connect on:", _serverFDToStringHost[fd]);
            this->ClientHandShake(it->second);
            hasHandShake = true;
    } else {
        _logger->Log(&Logger::LogCaution, "Error on apply handshake");
    }
    return hasHandShake;
}

ssize_t    Http::HandleRequest(int client_fd, int poll_fd) {
    char        buffer[1000000];
    HttpRequest res;
    IServer      *server = this->clientFD_Server[client_fd];

    memset(&buffer, 0, sizeof(char) * 1000000);
    ssize_t numbytes = recv(client_fd, &buffer, sizeof(char) * 1000000, 0);
    res.ParserRequest(buffer);
    if (res.IsCGIRequest()){
        int sv[2]; 
        if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv) == -1) {
            std::cerr << "Erro ao criar socket pair: " << strerror(errno) << std::endl;
            return -1;
        }
        this->_cgis[sv[0]] = client_fd;
        if (server->ProcessRequest(res, client_fd, sv, this->GetEPollFD()) == HttpStatusCode::_CGI) {
            return numbytes;
        }
    } else if (!res.IsCGIRequest() && server != NULL){
        server->ProcessRequest(res, client_fd);
    }
    (void)poll_fd;
    return numbytes;
}

void    Http::HandleResponse(int client_fd) {
    IServer *server = this->clientFD_Server[client_fd];
    IHttpResponse *message = server->ProcessResponse(client_fd);

    std::vector<char> test = message->CreateResponse();
    if (send(client_fd, &test[0], test.size(), 0) == -1) {
        //delete message;
        throw Except("Error on send Response");
    }
    //delete message;
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
    std::cout << _logger->Log(&ILogger::LogInformation, "Connect client", client_fd, "from server", server->GetIP(), "on port", server->GetPort());
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