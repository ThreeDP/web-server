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
                bool hasHandShake = false;
                std::map<std::string, Server*>::iterator it = this->servers.begin();
                for (; it != this->servers.end(); ++it) {
                    if (this->clientEvents[i].data.fd == it->second->GetListener()) {
                        this->ClientHandShake(it->second);
                        hasHandShake = true;
                        break;
                    }
                }
                if (hasHandShake)
                    continue;
                // HttpRequest_t recv_msg;
                char buffer[1000000];
                memset(&buffer, 0, sizeof(HttpRequest_t));
                ssize_t numbytes = recv(this->clientEvents[i].data.fd, &buffer, (int)sizeof(HttpRequest_t), 0);
                // std::cout << "method: " << recv_msg.method << " target: " << recv_msg.target << std::endl;
                if (numbytes == -1)
                    throw Except("error recv");
                else if (numbytes == 0) {
                    // connection closed by client
                    std::cerr << "Socket " << this->clientEvents[i].data.fd << " closed by client\n" << std::endl;
                    // delete fd from epoll
                    struct epoll_event  event;
                    event.data.fd = this->clientEvents[i].data.fd;
                    event.events = EPOLLIN;
                    if (epoll_ctl (this->GetEPollFD(), EPOLL_CTL_DEL, this->clientEvents[i].data.fd, &event) == -1)
                        throw Except("epoll_ctl");
                    if (close(this->clientEvents[i].data.fd) == -1)
                        throw Except("close");

                } else if (numbytes > 0) {
                    std::cout << "START SEND DATA!" << std::endl;
                    HttpResponse send_msg;
                    memset(&send_msg, '\0', sizeof(HttpResponse));
                    // Obter a hora atual
                    std::time_t now = std::time(NULL);
                    std::tm* gmt_time = std::gmtime(&now);
                    std::strftime(send_msg.date, sizeof(send_msg.date), "%Y-%m-%d %H:%M:%S GMT", gmt_time);
                    strcpy(send_msg.statusCode, "HTTP/1.1 200 OK\r\n");
                    strcpy(send_msg.contentType, "Content-Type: text/html\r\n");
                    strcpy(send_msg.contentLegth, "Content-Length: 65\r\n\r\n");
                    strcpy(send_msg.body, "<!DOCTYPE html><html><body><h1>Hello World!</h1></body></html>");

                    std::string httpResponse = std::string(send_msg.statusCode) +
                               std::string(send_msg.date) +
                               std::string(send_msg.contentType) +
                               std::string(send_msg.contentLegth) +
                               std::string(send_msg.body);
                    if (send(this->clientEvents[i].data.fd, httpResponse.c_str(), sizeof(HttpResponse), 0) == -1) {
                        throw Except("Error on send Response");
                    }
                    break;
                }
            }
            break;
        }
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