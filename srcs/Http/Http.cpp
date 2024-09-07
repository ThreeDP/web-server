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

void Http::Process(void) {
    int epollFD = epoll_create(1);
    if (epollFD == -1) {
        throw std::runtime_error(_logger->Log(&Logger::LogCaution, "Error: On start EPOLL."));
    }
	struct epoll_event event;
	memset(&event, '\0', sizeof(struct epoll_event));

	struct addrinfo                         hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_CANONNAME;
	
	struct addrinfo                         *result = NULL;

	// SET ENDEREÇO E PORTA
	int status = getaddrinfo("localhost", "8081", &hints, &result);
	if (status != 0) {
        close(epollFD);
        std::cerr << _logger->Log(&Logger::LogTrace, gai_strerror(status));
        throw std::runtime_error(_logger->Log(&Logger::LogCaution, "Error: Host <", "locahost", "Port", "8081", "> not found."));
    }

	// BIND DA FD DO SERVER COM A PORTA E HOST
	int optval = 1;
	int listener = 1;
	for (; result != NULL; result = result->ai_next) {
		listener = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
		if (listener == -1)
			continue;
		if (setsockopt(listener, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(int)) == -1) {
            std::cerr << _logger->Log(&Logger::LogWarning, "setsockopt.") << std::endl;
		}
		if (bind(listener, result->ai_addr, result->ai_addrlen) == 0) {
			break;
		}
		if (close(listener) == -1)
			std::cerr << _logger->Log(&Logger::LogWarning, "Problem on close socket.") << std::endl;
	}
	if (result == NULL) {
        close(listener);
        close(epollFD);
        throw std::runtime_error(_logger->Log(&Logger::LogCaution, "Error: Unable to bind <", "locahost", "Port", "8081", ">."));
    }

	// COMEÇA A OUVIR NO HOST E PORTA
	if (listen(listener, 10) == -1) {
        close(listener);
        close(epollFD);
        throw std::runtime_error(_logger->Log(&Logger::LogCaution, "Error: Unable to listen <", "locahost", "Port", "8081", ">."));
	}

	// ADICIONA O SERVER NO EPOLL
	event.events = EPOLLIN;
	event.data.fd = listener;
	if (epoll_ctl(epollFD, EPOLL_CTL_ADD, listener, &event) == -1) {
        close(listener);
        close(epollFD);
        throw std::runtime_error(_logger->Log(&Logger::LogCaution, "Error: Unable to execute EPOLL_CTL_ADD on <", "locahost", "Port", "8081", ">."));
	}

    _serverFDToServer[listener] = servers["localhost"];

	while (true) {
        // ESPERA NOVOS CLIENTES
		struct epoll_event  clientEvents[100];
		int number_of_ready_fds = epoll_wait(epollFD, clientEvents, 100, 2000);
		if (number_of_ready_fds == -1) {
            close(listener);
            close(epollFD);
            throw std::runtime_error(_logger->Log(&Logger::LogCaution, "Error: Problem to handle epoll clients."));
		}
        std::cout << _logger->Log(&Logger::LogInformation, "Check Status...");
		for (int i = 0; i < number_of_ready_fds; i++) {
			int new_socket = -1;
			socklen_t                   addrlen;
			struct sockaddr_storage     client_saddr;

            // HANDSHAKE
			addrlen = sizeof(struct sockaddr_storage);
            if (clientEvents[i].data.fd == listener) {

                if ((new_socket = accept(listener, (struct sockaddr *) &client_saddr, &addrlen)) < 0) {
                    std::cerr << _logger->Log(&Logger::LogWarning, "Problem to accept client.") << std::endl;
                }

                struct epoll_event event;
	            memset(&event, '\0', sizeof(struct epoll_event));
                event.events = EPOLLIN;
	            event.data.fd = new_socket;
                if (epoll_ctl(epollFD, EPOLL_CTL_ADD, new_socket, &event) == -1) {
                    close(new_socket);
                    std::cerr << _logger->Log(&Logger::LogWarning, "Problem to execute EPOLL_CTL_ADD to client: [", new_socket, "].") << std::endl;
                }
                clientFD_Server[new_socket] = _serverFDToServer[listener];
                std::cout << _logger->Log(&Logger::LogInformation, "HandShake client [",  new_socket, "] connected on", "localhost", "8081");
                continue;
            }

            // LER DO CLIENTE E ADICIONAR FD COMO EPOLLIN
            if (clientEvents[i].events & EPOLLIN) {
                HttpRequest req;
                char request[BUFFER_SIZE];

                memset(request, '\0', sizeof(char) * BUFFER_SIZE);
                recv(clientEvents[i].data.fd, request, sizeof(char) * BUFFER_SIZE, 0);
                std::cout << _logger->Log(&Logger::LogTrace, request);
                std::cout << _logger->Log(&Logger::LogInformation, "Request received from client [",  clientEvents[i].data.fd, "] connected on", "localhost", "8081");
                req.ParserRequest(request);
                clientFD_Server[clientEvents[i].data.fd]->ProcessRequest(req, clientEvents[i].data.fd, 0, epollFD);

                struct epoll_event event;
	            memset(&event, '\0', sizeof(struct epoll_event));
                event.events = EPOLLOUT;
	            event.data.fd = clientEvents[i].data.fd;
                if (epoll_ctl(epollFD, EPOLL_CTL_MOD, clientEvents[i].data.fd, &event) == -1) {
                    std::cerr << _logger->Log(&Logger::LogWarning, "Problem to execute EPOLL_CTL_MOD to client: [", clientEvents[i].data.fd, "].") << std::endl;
                    clientFD_Server.erase(clientEvents[i].data.fd);
                    close(clientEvents[i].data.fd);
                }
                std::cout << _logger->Log(&Logger::LogInformation, "Client [",  clientEvents[i].data.fd, "] connected on", "localhost", "8081");
            }

            // ESCREVO PARA O CLIENTE, DELETO FD DO EPOLL E FECHO O FD 
            if (clientEvents[i].events & EPOLLOUT) {
                IHttpResponse* res = clientFD_Server[clientEvents[i].data.fd]->ProcessResponse(clientEvents[i].data.fd);
                std::vector<char> response = res->CreateResponse();
                send(clientEvents[i].data.fd, &response[0], sizeof(char) * response.size(), 0);
                std::cout << _logger->Log(&Logger::LogInformation, "Send Response to client [",  clientEvents[i].data.fd, "] connected on", "localhost", "8081");
			    delete res;

                if (epoll_ctl(epollFD, EPOLL_CTL_DEL, clientEvents[i].data.fd, NULL) == -1) {
                    std::cerr << _logger->Log(&Logger::LogWarning, "Problem to execute EPOLL_CTL_DEL to client: [", clientEvents[i].data.fd, "].") << std::endl;
                    clientFD_Server.erase(clientEvents[i].data.fd);
                    close(clientEvents[i].data.fd);
                }
                std::cout << _logger->Log(&Logger::LogInformation, "Client [",  clientEvents[i].data.fd, "] disconnected from", "localhost", "8081");
                clientFD_Server.erase(clientEvents[i].data.fd);
                close(clientEvents[i].data.fd);
            }

		}
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
        event.events = EPOLLIN | EPOLLOUT;
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

            if ((this->clientEvents[i].events & EPOLLOUT)) {
                this->HandleResponse(this->clientEvents[i].data.fd);
                this->DisconnectClientToServer(this->clientEvents[i].data.fd);
                break;
            }

            if ((this->clientEvents[i].events & EPOLLIN) == EPOLLIN) {

                if (it != _cgis.end()) {
                    clientFD_Server[it->second]->CreateCGIResponse(this->GetEPollFD(), it->first, it->second);
                    _cgis.erase(it->first);
                    break;
                }
                
                if (this->ConnectClientToServer(this->clientEvents[i].data.fd))
                    continue;

                ssize_t numbytes = this->HandleRequest(this->clientEvents[i].data.fd, this->GetEPollFD());
                if (numbytes == -1)
                    throw Except("error recv");
                if (numbytes == 0) {
                    this->DisconnectClientToServer(this->clientEvents[i].data.fd);
                }  
                if (numbytes > 0) {
                    epoll_event ev;
                    memset(&ev, '\0', sizeof(epoll_event));
                    ev.events = EPOLLOUT;
                    ev.data.fd = this->clientEvents[i].data.fd;
                    if (epoll_ctl(this->GetEPollFD(), EPOLL_CTL_MOD, this->clientEvents[i].data.fd, &ev) == -1)
                        throw Except("Error on add server: <name> port: <port>");
                    break ;
                }

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
    char            buffer[1000000];
    HttpRequest     res;
    IServer         *server = this->clientFD_Server[client_fd];

    memset(&buffer, 0, sizeof(char) * 1000000);
    ssize_t numbytes = recv(client_fd, &buffer, sizeof(char) * 1000000, 0);
    res.ParserRequest(buffer);

    int sv[2]; 
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv) == -1) {
        std::cerr << "Erro ao criar socket pair: " << strerror(errno) << std::endl;
        return -1;
    }
    this->_cgis[sv[0]] = client_fd;
    if (server->ProcessRequest(res, client_fd, sv, this->GetEPollFD()) == HttpStatusCode::_CGI) {
        return numbytes;
    } else {
        close(sv[0]);
        close(sv[1]);
    }
    (void)poll_fd;
    delete server;
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
    std::cout << _logger->Log(&Logger::LogTrace, "Destruction Http.");
    if (!_serversPointer.empty()) {
        std::vector<IServer*>::iterator it = _serversPointer.begin();
        for (; it != _serversPointer.end(); ++it) {
          
            delete *it;
        }
    }
}