# include "Http.hpp"

void Http::Process(void) {
    int epollFD = epoll_create(1);
    if (epollFD == -1) {
        throw std::runtime_error(_logger->Log(&Logger::LogCaution, "Error: On start EPOLL."));
    }

	struct addrinfo                         hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_CANONNAME;
	
    // SET SERVERS HOSTS AND PORTS
    std::cout << _logger->Log(&Logger::LogInformation, "Setup Hosts and Ports.");

    std::vector<IServer *>::iterator itServer = _serversPointer.begin();
    for (; itServer != _serversPointer.end(); ++itServer) {

        std::vector<std::string>::iterator host = (*itServer)->GetListHosts().begin();
        for (; host != (*itServer)->GetListHosts().end(); ++host) {

            _result = NULL;
            // SET ENDEREÇO E PORTA
            std::string host_label = *host;
            std::string port_label = (*itServer)->GetListenPort();
            std::cout << _logger->Log(&Logger::LogInformation, "Set Host:", host_label, "and Port:", port_label);

            int status = getaddrinfo(host_label.c_str(), port_label.c_str(), &hints, &_result);
            if (status != 0) {
                CleanFds(epollFD);
                freeaddrinfo(_result);
                _result = NULL;
                std::cerr << _logger->Log(&Logger::LogTrace, gai_strerror(status));
                throw std::runtime_error(_logger->Log(&Logger::LogCaution, "Error: Host <", host_label, "Port:", port_label, "> not found."));
            }

            // BIND DA FD DO SERVER COM A PORTA E HOST
            int optval = 1;
            int listener = 1;
            std::cout << _logger->Log(&Logger::LogInformation, "Try to bind Host:", host_label, "and Port:", port_label);
            for (; _result != NULL; _result = _result->ai_next) {
                listener = socket(_result->ai_family, _result->ai_socktype, _result->ai_protocol);
                if (listener == -1)
                    continue;
                if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &optval, sizeof(int)) == -1) {
                    std::cerr << _logger->Log(&Logger::LogWarning, "setsockopt.") << std::endl;
                }
                if (bind(listener, _result->ai_addr, _result->ai_addrlen) == 0) {
                    break;
                }
                if (close(listener) == -1)
                    std::cerr << _logger->Log(&Logger::LogWarning, "Problem on close socket.") << std::endl;
            }

            if (_result == NULL) {
                freeaddrinfo(_result);
                _result = NULL;
                CleanFds(epollFD);
                close(listener);
                throw std::runtime_error(_logger->Log(&Logger::LogCaution, "Error: Unable to bind <", host_label, "Port:", port_label, ">."));
            }
            freeaddrinfo(_result);
            _result = NULL;

            std::cout << _logger->Log(&Logger::LogInformation, "Server: [", listener, "] bind Host:", host_label, "and Port:", port_label);
            std::cout << _logger->Log(&Logger::LogInformation, "Server: [", listener, "] try to starting listing on Host:", host_label, "and Port:", port_label);

        	// COMEÇA A OUVIR NO HOST E PORTA
            if (listen(listener, 10) == -1) {
                CleanFds(epollFD);
                close(listener);
                throw std::runtime_error(_logger->Log(&Logger::LogCaution, "Error: Unable to listen <", host_label, "Port:", port_label, ">."));
            }
            std::cout << _logger->Log(&Logger::LogInformation, "Server: [", listener, "] listining on Host:", host_label, "and Port:", port_label);

            // ADICIONA O SERVER NO EPOLL
            struct epoll_event event;
            memset(&event, '\0', sizeof(struct epoll_event));
            event.events = EPOLLIN;
            event.data.fd = listener;
            if (epoll_ctl(epollFD, EPOLL_CTL_ADD, listener, &event) == -1) {
                CleanFds(epollFD);
                close(listener);
                throw std::runtime_error(_logger->Log(&Logger::LogCaution, "Error: Unable to execute EPOLL_CTL_ADD on <", "locahost", "Port", "8081", ">."));
            }
            std::cout << _logger->Log(&Logger::LogInformation, "ADD Server: [", listener, "] with EPOLL_CTL_ADD Hosts:", host_label, "and Port:", port_label);

            _serverFDToServer[listener] = *itServer;
        }
    }
    if (_serverFDToServer.empty()) {
        throw std::runtime_error(_logger->Log(&Logger::LogCaution, "Error: Unable to Start Server Listen."));
    }

    struct epoll_event  clientEvents[10];
	while (true) {
		int number_of_ready_fds = epoll_wait(epollFD, clientEvents, 10, 1000);
		if (number_of_ready_fds == -1) {
            CleanFds(epollFD);
            throw std::runtime_error(_logger->Log(&Logger::LogCaution, "Error: Problem to handle epoll clients."));
		}

        std::cout << _logger->Log(&Logger::LogInformation, "Check Status... ", "ready fds: [", number_of_ready_fds, "]");
		for (int i = 0; i < number_of_ready_fds; i++) {
            if (HandShake(epollFD, clientEvents[i]) == true)
                continue;
            if (CGIReadResponse(epollFD, clientEvents[i]) == true)
                continue;
            if (CGIWriteRequest(epollFD, clientEvents[i]) == true)
                break;
            if ((clientEvents[i].events & EPOLLIN)) {
                bool Continue = (!_clientFDToClient[clientEvents[i].data.fd].Request.empty()) ? true : false;
                //bool Continue = (_clientFDToRequest.find(clientEvents[i].data.fd) != _clientFDToRequest.end()) ? true : false;
                std::vector<char> requestBytes = ReadRequest(epollFD, clientEvents[i]);
                if (requestBytes.empty())
                    break;
                HttpRequest req;
                _clientFDToClient[clientEvents[i].data.fd].Request.insert(
                    _clientFDToClient[clientEvents[i].data.fd].Request.end(),
                    requestBytes.begin(),
                    requestBytes.end()
                );
                // _clientFDToRequest[clientEvents[i].data.fd].insert(_clientFDToRequest[clientEvents[i].data.fd].end(), requestBytes.begin(), requestBytes.end());
                req.ParserRequest(_clientFDToClient[clientEvents[i].data.fd].Request);
                // req.ParserRequest(_clientFDToRequest[clientEvents[i].data.fd]);
                
                std::cout << _logger->Log(&Logger::LogInformation, "Request received from client [",  clientEvents[i].data.fd, "] connected on", "localhost", "8081");
                if (ProcessResponse(epollFD, clientEvents[i], requestBytes.size(), req, Continue) == true)
                    continue;
            }
            if (WriteResponse(epollFD, clientEvents[i]) == true)
                break;
		}
        //CheckTimeout(epollFD);
	}
}

void Http::CleanFds(int EpollFD) {
    std::map<int, IServer *>::iterator itFD = _serverFDToServer.begin();
    for ( ; itFD != _serverFDToServer.end(); ++itFD) {
        close(itFD->first);
    }
    close(EpollFD);
}

bool Http::HandShake(int EpollFD, struct epoll_event &clientEvent) {
    int new_socket = -1;
    socklen_t                   addrlen;
    struct sockaddr_storage     client_saddr;

    // HANDSHAKE
    addrlen = sizeof(struct sockaddr_storage);
    std::map<int, IServer *>::iterator itListener = _serverFDToServer.find(clientEvent.data.fd);
    if (itListener != _serverFDToServer.end()) {

        if ((new_socket = accept(itListener->first, (struct sockaddr *) &client_saddr, &addrlen)) > 0) {
            AddConnection(EpollFD, new_socket);
            _clientFDToClient[new_socket].Server = _serverFDToServer[itListener->first];
            // clientFD_Server[new_socket] = _serverFDToServer[itListener->first];
            std::cout << _logger->Log(&Logger::LogInformation, "HandShake client [",  new_socket, "] connected on", "localhost", "8081");
            return true;
        }
        std::cerr << _logger->Log(&Logger::LogWarning, "Problem to accept client.") << std::endl;
    }
    return false;
}

bool Http::CGIWriteRequest(int EpollFD, struct epoll_event &clientEvent) {
    std::map<int, int>::iterator it;
    if ((clientEvent.events & EPOLLIN) && (it = _cgis.find(clientEvent.data.fd)) != _cgis.end()) {
        std::cout << _logger->Log(&Logger::LogInformation, "Entered CGI pollin: [", clientEvent.data.fd, "].");
        ModifyClientFDState(EpollFD, clientEvent.data.fd, EPOLLOUT);
        return true;
    }
    return false;
}

bool Http::CGIReadResponse(int EpollFD, struct epoll_event &clientEvent) {
    std::map<int, int>::iterator it;
    if ((clientEvent.events & EPOLLOUT) && (it = _cgis.find(clientEvent.data.fd)) != _cgis.end()) {
        std::cout << _logger->Log(&Logger::LogInformation, "Entered CGI pollout: [", clientEvent.data.fd, "].");
        _clientFDToClient[it->second].Server->CreateCGIResponse(EpollFD, it->first, it->second);
        //clientFD_Server[it->second]->CreateCGIResponse(EpollFD, it->first, it->second);
        _cgis.erase(clientEvent.data.fd);
        close(clientEvent.data.fd);
        return true;
    }
    return false;
}

std::vector<char> Http::ReadRequest(int EpollFD, struct epoll_event &clientEvent) {
    HttpRequest req;
    char request[BUFFER_SIZE];
    memset(request, '\0', sizeof(char) * BUFFER_SIZE);

    std::cout << _logger->Log(&Logger::LogInformation, "Entered pollin: [", clientEvent.data.fd, "].");
    int numbytes = recv(clientEvent.data.fd, request, sizeof(char) * BUFFER_SIZE, 0);
    std::cout << _logger->Log(&Logger::LogTrace, "'", request, "'", "\nNumber of Bytes:", numbytes);
    
    if (numbytes == -1) {
        std::cerr << _logger->Log(&Logger::LogWarning, "Problem to RECV request of client: [", clientEvent.data.fd, "].");
        CloseConnection(EpollFD, clientEvent.data.fd);
        return (std::vector<char>());
    }

    if (numbytes == 0) {
        std::cerr << _logger->Log(&Logger::LogWarning, "Client: [", clientEvent.data.fd, "] close connection.");
        CloseConnection(EpollFD, clientEvent.data.fd);
        return std::vector<char>();
    }

    if (numbytes > 0) {
        return std::vector<char>(request, request + numbytes);
    }
    return std::vector<char>();
}

bool Http::ProcessResponse(int EpollFD, struct epoll_event &clientEvent, size_t numbytes, HttpRequest &Req, bool Continue) {
    int sv[2];
    memset(&sv, '\0', sizeof(sv));
    if (numbytes < BUFFER_SIZE) {
        if (Continue && Req._payload.find("Expect:") != Req._payload.end()) {
            Req._payload.erase("Expect:");
        }
        
        if (socketpair(AF_UNIX, SOCK_STREAM, 0, _clientFDToClient[clientEvent.data.fd].cgiPair) == -1) {
            std::cerr << _logger->Log(&Logger::LogWarning, "Problem to open socketpair: [", clientEvent.data.fd, "].") << std::endl;
        }
        
        int isCGI = (_clientFDToClient[clientEvent.data.fd].Server->ProcessRequest(Req, clientEvent.data.fd, _clientFDToClient[clientEvent.data.fd].cgiPair, EpollFD) == HttpStatusCode::_CGI);
        // int isCGI = (clientFD_Server[clientEvent.data.fd]->ProcessRequest(Req, clientEvent.data.fd, sv, EpollFD) == HttpStatusCode::_CGI);

        // MODIFICA PARA EPOLLOUT
        ModifyClientFDState(EpollFD, clientEvent.data.fd, EPOLLOUT);

        std::cout << _logger->Log(&Logger::LogInformation, "Client [",  clientEvent.data.fd, "] connected on", "localhost", "8081");
        if (isCGI) {
            AddConnection(EpollFD, _clientFDToClient[clientEvent.data.fd].cgiPair[0]);
            this->_cgis[_clientFDToClient[clientEvent.data.fd].cgiPair[0]] = clientEvent.data.fd;
            return true;
        }
        close(_clientFDToClient[clientEvent.data.fd].cgiPair[0]);
        close(_clientFDToClient[clientEvent.data.fd].cgiPair[1]);
    }
    return false;
}

bool Http::WriteResponse(int EpollFD, struct epoll_event &clientEvent) {
    // ESCREVO PARA O CLIENTE, DELETO FD DO EPOLL E FECHO O FD 
    // if ((clientEvent.events & EPOLLOUT) && clientFD_Server[clientEvent.data.fd]->FindResponse(clientEvent.data.fd)) {
    if ((clientEvent.events & EPOLLOUT) && _clientFDToClient[clientEvent.data.fd].Server->FindResponse(clientEvent.data.fd)) {
        std::cout << _logger->Log(&Logger::LogInformation, "Entered pollout: [", clientEvent.data.fd, "].");
        IHttpResponse* res = _clientFDToClient[clientEvent.data.fd].Server->ProcessResponse(clientEvent.data.fd);
        // IHttpResponse* res = clientFD_Server[clientEvent.data.fd]->ProcessResponse(clientEvent.data.fd);
        
        std::vector<char> response = res->CreateResponse();
        // TRY TO SEND MESSAGE
        int numbytes = send(clientEvent.data.fd, &response[0], sizeof(char) * response.size(), 0);
        if (numbytes == -1 || numbytes == 0) {
            std::cerr << _logger->Log(&Logger::LogWarning, "Problem to SEND response of client: [", clientEvent.data.fd, "].") << std::endl;
            CloseConnection(EpollFD, clientEvent.data.fd);
            delete res;
            return true;
        }

        std::cout << _logger->Log(&Logger::LogInformation, "Send Response to client [", clientEvent.data.fd, "] connected on", "localhost", "8081");
        std::string StatusCode = res->GetStatusCode();
        delete res;
        if (StatusCode == "100") {
            std::cout << _logger->Log(&Logger::LogInformation, "100-Continue Confirmation [",  clientEvent.data.fd, "]", "localhost", "8081");
            // MODIFICA PARA EPOLLIN
            ModifyClientFDState(EpollFD, clientEvent.data.fd, EPOLLIN);
            return true;
        }
        // CLOSE CONNECTION
        CloseConnection(EpollFD, clientEvent.data.fd);
        std::cout << _logger->Log(&Logger::LogInformation, "Client [",  clientEvent.data.fd, "] disconnected from", "localhost", "8081");
    }
    return false;
}

void Http::CloseConnection(int EpollFD, int clientEvents_fd) {
    if (epoll_ctl(EpollFD, EPOLL_CTL_DEL, clientEvents_fd, NULL) == -1) {
        std::cerr << _logger->Log(&Logger::LogWarning, "Problem to execute EPOLL_CTL_DEL to client: [", clientEvents_fd, "].") << std::endl;
    }
    _clientFDToClient.erase(clientEvents_fd);
    _cgis.erase(clientEvents_fd);
    //_clientFDToRequest.erase(clientEvents_fd);
    //clientFD_Server.erase(clientEvents_fd);
    close(clientEvents_fd);
}

void Http::AddConnection(int EpollFD, int client_fd) {
    struct epoll_event event;
    memset(&event, '\0', sizeof(struct epoll_event));
    event.events = EPOLLIN;
    event.data.fd = client_fd;

    if (epoll_ctl(EpollFD, EPOLL_CTL_ADD, client_fd, &event) == -1) {
        close(client_fd);
        std::cerr << _logger->Log(&Logger::LogWarning, "Problem to execute EPOLL_CTL_ADD to client: [", client_fd, "].") << std::endl;
    }
}

void Http::ModifyClientFDState(int EpollFD, int clientEvents_fd, uint32_t epollState) {
    struct epoll_event event;
    memset(&event, '\0', sizeof(struct epoll_event));
    event.events = epollState;
    event.data.fd = clientEvents_fd;

    if (epoll_ctl(EpollFD, EPOLL_CTL_MOD, clientEvents_fd, &event) == -1) {
        std::cerr << _logger->Log(&Logger::LogWarning, "Problem to execute EPOLL_CTL_MOD to client: [", clientEvents_fd, "].") << std::endl;
        CloseConnection(EpollFD, clientEvents_fd);
    }
}

void    Http::CheckTimeout(int EpollFD)
{
    for(std::map<int, Client>::iterator it = _clientFDToClient.begin() ; it != _clientFDToClient.end(); ++it)
    {
        if (it->second.CheckIfExpire())
        {
            std::cerr << _logger->Log(&Logger::LogWarning, "Client: [", it->first, "] has close by timeout.");
            CloseConnection(EpollFD, it->first);
        }
    }
}

// ssize_t    Http::HandleRequest(int clientEvents[i].data.fd, int poll_fd) {
//     char            buffer[1000000];
//     HttpRequest     res;
//     IServer         *server = this->clientFD_Server[clientEvents[i].data.fd];

//     memset(&buffer, 0, sizeof(char) * 1000000);
//     ssize_t numbytes = recv(clientEvents[i].data.fd, &buffer, sizeof(char) * 1000000, 0);
//     res.ParserRequest(buffer);

//     int sv[2]; 
//     if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv) == -1) {
//         std::cerr << "Erro ao criar socket pair: " << strerror(errno) << std::endl;
//         return -1;
//     }
//     this->_cgis[sv[0]] = clientEvents[i].data.fd;
//     if (server->ProcessRequest(res, clientEvents[i].data.fd, sv, this->GetEPollFD()) == HttpStatusCode::_CGI) {
//         return numbytes;
//     } else {
//         close(sv[0]);
//         close(sv[1]);
//     }
//     (void)poll_fd;
//     delete server;
//     return numbytes;
// }

/* Geters
=================================================*/
IServer *Http::GetServer(std::string server) {
    std::map<std::string, IServer *>::iterator it = this->servers.find(server);
    if (it != this->servers.end()) {
        return it->second;
    }
    return NULL;
}

/* Seters
=================================================*/
void Http::SetServer(std::string serverName, IServer *server) {
    this->servers[serverName] = server;
}

void Http::SetServer(IServer *server) {
    std::vector<std::string> hosts = server->GetListHosts();
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
    _serversPointer.clear();
    _clientFDToClient.clear();
    // _clientFDToRequest.clear();
    _cgis.clear();
    _logger = logger;
    _result = NULL;
}

Http::~Http(void) {
    std::cout << _logger->Log(&Logger::LogTrace, "Destruction Http.");
    if (!_serversPointer.empty()) {
        std::vector<IServer*>::iterator it = _serversPointer.begin();
        for (; it != _serversPointer.end(); ++it) {
          
            delete *it;
        }
    }
    if (_result != NULL) {
        freeaddrinfo(_result);
        _result = NULL;
    }
}