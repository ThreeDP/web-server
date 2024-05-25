# include "Http.hpp"

/* Http Methods
=================================================*/
void    Http::StartPollList(void) {
    struct epoll_event  event;

    int &epollFD = this->GetEPollFD();
    if (epollFD == -1) {
        throw Except("Error on create Epoll");
    }
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
                ssize_t numbytes = this->HandleRequest();  
                if (numbytes == -1)
                    throw Except("error recv");
                else if (numbytes == 0) {
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

ssize_t    Http::HandleRequest(int client_fd) {
    char buffer[1000000];
    memset(&buffer, 0, sizeof(HttpRequest_t));
    ssize_t numbytes = recv(client_fd, &buffer, sizeof(char) * 1000000, 0);
    std::cout << "header: \n" << std::string(buffer) << std::endl;
    return numbytes;
}

void    Http::DisconnectClientToServer(int client_fd) {
    std::cerr << "Socket " << client_fd << " closed by client\n" << std::endl;
    struct epoll_event  event;
    event.data.fd = client_fd;
    event.events = EPOLLIN;

    this->clientFD_Server.erase(client_fd);
    if (epoll_ctl (this->GetEPollFD(), EPOLL_CTL_DEL, client_fd, &event) == -1)
        throw Except("epoll_ctl");
    if (close(client_fd) == -1)
        throw Except("close");
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

void    Http::HandleResponse(int client_fd) {
    // Envia pra o server processar a resposta.
    Server *server = this->clientFD_Server[client_fd];
    std::string message = server->ProcessResponse();
    // Realiza o envio para o client.
    if (send(client_fd, message.c_str(), message.size(), 0) == -1) {
        throw Except("Error on send Response");
    }
}

void    Http::ClientHandShake(Server *server) {
    int                         client_fd;
    socklen_t                   addrlen;
    struct epoll_event          event;
    struct sockaddr_storage     client_saddr;
    std::cout << "HandShake" << std::endl;
    memset(&event, 0, sizeof(struct epoll_event));
    addrlen = sizeof(struct sockaddr_storage);

    client_fd = accept(
        server->GetListener(),
        (struct sockaddr *) &client_saddr,
        &addrlen
    );
    if (client_fd == -1) {
        throw Except("Error on accept client");
    }
    event.events = EPOLLIN;
    event.data.fd = client_fd;
    if (epoll_ctl(this->GetEPollFD(), EPOLL_CTL_ADD, client_fd, &event) == 1) {
        throw Except("Error on add client on epoll.");
    }
    this->clientFD_Server[client_fd] = server;
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
    static int  epollFD = epoll_create1(0);
    return epollFD;
}

/* Seters
=================================================*/
void Http::SetServer(std::string serverName, Server *server) {
    this->servers[serverName] = server;
}

/* Base Methods
=================================================*/
Http::Http(void) {

}

Http::~Http(void) {

}