# include "Http.hpp"

/* Http Methods
=================================================*/
void    Http::StartPollList(void) {
    struct epoll_event  event;

    this->_stage = H_ADD_SERVERS;
    int &epollFD = this->GetEPollFD();
    if (epollFD == -1) {
        throw Except("Error on create Epoll");
    }
    std::cout << *this;    
    std::map<std::string, Server*>::iterator it = this->servers.begin();
    for (; it != this->servers.end(); ++it) {
        memset(&event, 0, sizeof(struct epoll_event));
        it->second->SetAddrInfo();
        it->second->CreateSocketAndBind();
        event.data.fd = it->second->StartListen();
        event.events = EPOLLIN;
        if (epoll_ctl(epollFD, EPOLL_CTL_ADD, it->second->GetListener(), &event) == -1)
            throw Except("Error on add server: <name> port: <port>");
    }
}

void    Http::StartWatchSockets(void) {
    this->_stage = H_STAND_BY;
    std::cout << *this;
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
    Server *server = this->clientFD_Server[client_fd];
    server->UpdateState(S_CLIENT_DISCONNECT, client_fd);
    this->clientFD_Server.erase(client_fd);
    std::cout << *server; 
}

bool    Http::ConnectClientToServer(int i) {
    bool hasHandShake = false;
    std::map<std::string, Server*>::iterator it = this->servers.begin();
    // Busca o Server compativel com o fd do request.
    for (; it != this->servers.end(); ++it) {
        if (this->clientEvents[i].data.fd == it->second->GetListener()) {
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
    Server      *server = this->clientFD_Server[client_fd];

    memset(&buffer, 0, sizeof(char) * 1000000);
    ssize_t numbytes = recv(client_fd, &buffer, sizeof(char) * 1000000, 0);
    res.ParserRequest(buffer);
    server->ProcessRequest(res, client_fd);
    return numbytes;
}

void    Http::HandleResponse(int client_fd) {
    // Envia pra o server processar a resposta.
    Server *server = this->clientFD_Server[client_fd];
    std::string message = server->ProcessResponse(client_fd);
    // Realiza o envio para o client.
    if (send(client_fd, message.c_str(), message.size(), 0) == -1) {
        throw Except("Error on send Response");
    }
    // std::cout << *server;
}

void    Http::ClientHandShake(Server *server) {
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
    std::cout << *server;
}

/* Geters
=================================================*/
Server *Http::GetServer(std::string server) {
    std::map<std::string, Server *>::iterator it = this->servers.find(server);
    if (it != this->servers.end()) {
        return it->second;
    }
    return NULL;
}

int &Http::GetEPollFD(void) {
    static int epollFD = epoll_create1(0);
    return epollFD;
}

HttpStages  Http::GetStage(void) const {
    return this->_stage;
}

/* Seters
=================================================*/
void Http::SetServer(std::string serverName, Server *server) {
    this->servers[serverName] = server;
}

/* Base Methods
=================================================*/
Http::Http(void) {
    this->_stage = H_START;
    HttpResponse::SetDefaultHTTPResponse();
    std::cout << *this;
}

Http::~Http(void) {

}

std::ostream &operator<<(std::ostream &os, Http const &http) {
    switch (http.GetStage()) {
    case H_START:
        os << HGRN "[ Starting service... ]" reset << std::endl;
        break;
    case H_CONFIG:
        os << HGRN "[ Reading configuration file and creating server configuration rules. ]" reset << std::endl;
        break;
    case H_ADD_SERVERS:
        os << HGRN "[ Adding services in listening mode... ]" reset << std::endl;
        break;
    case H_STAND_BY:
        os << HGRN "[ Observing changes to file descriptors... ]" reset << std::endl;
        break;
    default:
        os << "Error";
    }
	return (os);
}