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

            struct addrinfo                         *result = NULL;
            // SET ENDEREÇO E PORTA
            std::string host_label = *host;
            std::string port_label = (*itServer)->GetListenPort();
            std::cout << _logger->Log(&Logger::LogInformation, "Set Host:", host_label, "and Port:", port_label);

            int status = getaddrinfo(host_label.c_str(), port_label.c_str(), &hints, &result);
            if (status != 0) {
                freeaddrinfo(result);
                std::cerr << _logger->Log(&Logger::LogTrace, gai_strerror(status));
                std::map<int, IServer *>::iterator itFD = _serverFDToServer.begin();
                for ( ; itFD != _serverFDToServer.end(); ++itFD) {
                    close(itFD->first);
                }
                close(epollFD);
                throw std::runtime_error(_logger->Log(&Logger::LogCaution, "Error: Host <", host_label, "Port:", port_label, "> not found."));
            }

            // BIND DA FD DO SERVER COM A PORTA E HOST
            int optval = 1;
            int listener = 1;
            std::cout << _logger->Log(&Logger::LogInformation, "Try to bind Host:", host_label, "and Port:", port_label);
            for (; result != NULL; result = result->ai_next) {
                listener = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
                if (listener == -1)
                    continue;
                if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &optval, sizeof(int)) == -1) {
                    std::cerr << _logger->Log(&Logger::LogWarning, "setsockopt.") << std::endl;
                }
                if (bind(listener, result->ai_addr, result->ai_addrlen) == 0) {
                    break;
                }
                if (close(listener) == -1)
                    std::cerr << _logger->Log(&Logger::LogWarning, "Problem on close socket.") << std::endl;
            }

            if (result == NULL) {
                std::map<int, IServer *>::iterator itFD = _serverFDToServer.begin();
                freeaddrinfo(result);
                for ( ; itFD != _serverFDToServer.end(); ++itFD) {
                    close(itFD->first);
                }
                close(listener);
                close(epollFD);
                throw std::runtime_error(_logger->Log(&Logger::LogCaution, "Error: Unable to bind <", host_label, "Port:", port_label, ">."));
            }

            freeaddrinfo(result);

            std::cout << _logger->Log(&Logger::LogInformation, "Server: [", listener, "] bind Host:", host_label, "and Port:", port_label);
            std::cout << _logger->Log(&Logger::LogInformation, "Server: [", listener, "] try to starting listing on Host:", host_label, "and Port:", port_label);

        	// COMEÇA A OUVIR NO HOST E PORTA
            if (listen(listener, 10) == -1) {
                std::map<int, IServer *>::iterator itFD = _serverFDToServer.begin();
                for ( ; itFD != _serverFDToServer.end(); ++itFD) {
                    close(itFD->first);
                }
                close(listener);
                close(epollFD);
                throw std::runtime_error(_logger->Log(&Logger::LogCaution, "Error: Unable to listen <", host_label, "Port:", port_label, ">."));
            }
            std::cout << _logger->Log(&Logger::LogInformation, "Server: [", listener, "] listining on Host:", host_label, "and Port:", port_label);

            // ADICIONA O SERVER NO EPOLL
            struct epoll_event event;
            memset(&event, '\0', sizeof(struct epoll_event));
            event.events = EPOLLIN;
            event.data.fd = listener;
            if (epoll_ctl(epollFD, EPOLL_CTL_ADD, listener, &event) == -1) {
                std::map<int, IServer *>::iterator itFD = _serverFDToServer.begin();
                for ( ; itFD != _serverFDToServer.end(); ++itFD) {
                    close(itFD->first);
                }
                close(listener);
                close(epollFD);
                throw std::runtime_error(_logger->Log(&Logger::LogCaution, "Error: Unable to execute EPOLL_CTL_ADD on <", "locahost", "Port", "8081", ">."));
            }
            std::cout << _logger->Log(&Logger::LogInformation, "ADD Server: [", listener, "] with EPOLL_CTL_ADD Hosts:", host_label, "and Port:", port_label);

            _serverFDToServer[listener] = *itServer;
        }
    }
    if (_serverFDToServer.empty()) {
        throw std::runtime_error(_logger->Log(&Logger::LogCaution, "Error: Unable to Start Server Listen."));
    }

    struct epoll_event  clientEvents[100];
	while (true) {
        // ESPERA NOVOS CLIENTES
		int number_of_ready_fds = epoll_wait(epollFD, clientEvents, 100, 1000);
		if (number_of_ready_fds == -1) {
            std::map<int, IServer *>::iterator itFD = _serverFDToServer.begin();
            for ( ; itFD != _serverFDToServer.end(); ++itFD) {
                close(itFD->first);
            }
            close(epollFD);
            throw std::runtime_error(_logger->Log(&Logger::LogCaution, "Error: Problem to handle epoll clients."));
		}

        std::cout << _logger->Log(&Logger::LogInformation, "Check Status... ", "ready fds: [", number_of_ready_fds, "]");
		for (int i = 0; i < number_of_ready_fds; i++) {
			int new_socket = -1;
			socklen_t                   addrlen;
			struct sockaddr_storage     client_saddr;

            // HANDSHAKE
			addrlen = sizeof(struct sockaddr_storage);
            std::map<int, IServer *>::iterator itListener = _serverFDToServer.find(clientEvents[i].data.fd);
            if (itListener != _serverFDToServer.end()) {

                if ((new_socket = accept(itListener->first, (struct sockaddr *) &client_saddr, &addrlen)) < 0) {
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
                clientFD_Server[new_socket] = _serverFDToServer[itListener->first];
                std::cout << _logger->Log(&Logger::LogInformation, "HandShake client [",  new_socket, "] connected on", "localhost", "8081");
                continue;
            }

            std::map<int, int>::iterator it = _cgis.find(clientEvents[i].data.fd);
            if (it != _cgis.end()) {
                clientFD_Server[it->second]->CreateCGIResponse(epollFD, it->first, it->second);
                _cgis.erase(clientEvents[i].data.fd);
                close(clientEvents[i].data.fd);
                break;
            }

            // LER DO CLIENTE E ADICIONAR FD COMO EPOLLIN
            if ((clientEvents[i].events & EPOLLIN)) {
                std::cout << _logger->Log(&Logger::LogInformation, "Entered pollin: [", clientEvents[i].data.fd, "].");
                HttpRequest req;
                char request[BUFFER_SIZE];
                memset(request, '\0', sizeof(char) * BUFFER_SIZE);
                int numbytes = recv(clientEvents[i].data.fd, request, sizeof(char) * BUFFER_SIZE, 0);
                
                // QUANDO OCORRE UM PROBLEMA AO EXECUTAR O RECV
                if (numbytes == -1) {
                    std::cerr << _logger->Log(&Logger::LogWarning, "Problem to RECV request of client: [", clientEvents[i].data.fd, "].") << std::endl;
                    _clientFDToRequest.erase(clientEvents[i].data.fd);
                    clientFD_Server.erase(clientEvents[i].data.fd);
                    close(clientEvents[i].data.fd);
                    break;
                }

                // QUANDO CLIENT FECHA NO MEIO DO REQUEST
                if (numbytes == 0) {
                    std::cerr << _logger->Log(&Logger::LogWarning, "Client: [", clientEvents[i].data.fd, "] close connection.") << std::endl;
                    if (epoll_ctl(epollFD, EPOLL_CTL_DEL, clientEvents[i].data.fd, NULL) == -1) {
                        std::cerr << _logger->Log(&Logger::LogWarning, "Problem to execute EPOLL_CTL_DEL to client: [", clientEvents[i].data.fd, "].") << std::endl;
                    }
                    _clientFDToRequest.erase(clientEvents[i].data.fd);
                    clientFD_Server.erase(clientEvents[i].data.fd);
                    close(clientEvents[i].data.fd);
                    break;
                }

                std::vector<char> vec(request, request + numbytes);

                bool Continue;
                Continue = (_clientFDToRequest.find(clientEvents[i].data.fd) != _clientFDToRequest.end()) ? true : false;
                _clientFDToRequest[clientEvents[i].data.fd].insert(_clientFDToRequest[clientEvents[i].data.fd].end(), request, request + numbytes);
                req.ParserRequest(_clientFDToRequest[clientEvents[i].data.fd]);

                std::cout << _logger->Log(&Logger::LogTrace, request);
                std::cout << _logger->Log(&Logger::LogInformation, "Request received from client [",  clientEvents[i].data.fd, "] connected on", "localhost", "8081");

                int sv[2];
                memset(&sv, '\0', sizeof(sv));
                if (numbytes < BUFFER_SIZE) {

                    if (Continue && req._payload.find("Expect:") != req._payload.end()) {
                        req._payload.erase("Expect:");
                    }
                    
                    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv) == -1) {
                        std::cerr << _logger->Log(&Logger::LogWarning, "Problem to open socketpair: [", clientEvents[i].data.fd, "].") << std::endl;
                    }
                    
                    int isCGI = (clientFD_Server[clientEvents[i].data.fd]->ProcessRequest(req, clientEvents[i].data.fd, sv, epollFD) == HttpStatusCode::_CGI);

                    struct epoll_event event;
                    memset(&event, '\0', sizeof(struct epoll_event));
                    event.events = EPOLLOUT;
                    event.data.fd = clientEvents[i].data.fd;

                    if (epoll_ctl(epollFD, EPOLL_CTL_MOD, clientEvents[i].data.fd, &event) == -1) {
                        std::cerr << _logger->Log(&Logger::LogWarning, "Problem to execute EPOLL_CTL_MOD to client: [", clientEvents[i].data.fd, "].") << std::endl;
                        _clientFDToRequest.erase(clientEvents[i].data.fd);
                        clientFD_Server.erase(clientEvents[i].data.fd);
                        close(clientEvents[i].data.fd);
                    }
                    std::cout << _logger->Log(&Logger::LogInformation, "Client [",  clientEvents[i].data.fd, "] connected on", "localhost", "8081");
                    if (isCGI) {
                        this->_cgis[sv[0]] = clientEvents[i].data.fd;

                        struct epoll_event event;
                        memset(&event, 0, sizeof(struct epoll_event));
                        event.events = EPOLLIN;
                        event.data.fd = sv[0];

                        if (epoll_ctl(epollFD, EPOLL_CTL_ADD, sv[0], &event) == -1) {
                            perror("Erro ao adicionar o descritor ao epoll");
                            exit(EXIT_FAILURE);
                        }
                        continue;
                    }
                    close(sv[0]);
                    close(sv[1]);
                }
            }


            // ESCREVO PARA O CLIENTE, DELETO FD DO EPOLL E FECHO O FD 
            if ((clientEvents[i].events & EPOLLOUT) && clientFD_Server[clientEvents[i].data.fd]->FindResponse(clientEvents[i].data.fd)) {
                IHttpResponse* res = clientFD_Server[clientEvents[i].data.fd]->ProcessResponse(clientEvents[i].data.fd);
                
                std::vector<char> response = res->CreateResponse();

                int numbytes = send(clientEvents[i].data.fd, &response[0], sizeof(char) * response.size(), 0);
                if (numbytes == -1) {
                    std::cerr << _logger->Log(&Logger::LogWarning, "Problem to SEND response of client: [", clientEvents[i].data.fd, "].") << std::endl;
                    _clientFDToRequest.erase(clientEvents[i].data.fd);
                    clientFD_Server.erase(clientEvents[i].data.fd);
                    close(clientEvents[i].data.fd);
			        delete res;
                }

                std::cout << _logger->Log(&Logger::LogInformation, "Send Response to client [",  clientEvents[i].data.fd, "] connected on", "localhost", "8081");

                if (res->GetStatusCode() == "100") {

                    struct epoll_event event;
                    memset(&event, '\0', sizeof(struct epoll_event));
                    event.events = EPOLLIN;
                    event.data.fd = clientEvents[i].data.fd;

                    if (epoll_ctl(epollFD, EPOLL_CTL_MOD, clientEvents[i].data.fd, &event) == -1) {
                        std::cerr << _logger->Log(&Logger::LogWarning, "Problem to execute EPOLL_CTL_MOD to client: [", clientEvents[i].data.fd, "].") << std::endl;
                        _clientFDToRequest.erase(clientEvents[i].data.fd);
                        _clientFDToRequest.erase(clientEvents[i].data.fd);
                        clientFD_Server.erase(clientEvents[i].data.fd);
                        close(clientEvents[i].data.fd);
                    }
			        delete res;
                    break;
                }
			    delete res;

                if (epoll_ctl(epollFD, EPOLL_CTL_DEL, clientEvents[i].data.fd, NULL) == -1) {
                    std::cerr << _logger->Log(&Logger::LogWarning, "Problem to execute EPOLL_CTL_DEL to client: [", clientEvents[i].data.fd, "].") << std::endl;
                }
                std::cout << _logger->Log(&Logger::LogInformation, "Client [",  clientEvents[i].data.fd, "] disconnected from", "localhost", "8081");
                _clientFDToRequest.erase(clientEvents[i].data.fd);
                clientFD_Server.erase(clientEvents[i].data.fd);
                close(clientEvents[i].data.fd);
            }
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
    _clientFDToRequest.clear();
    _cgis.clear();
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