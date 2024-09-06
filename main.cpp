# include <fcntl.h>
# include <sys/epoll.h>
# include <string.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netdb.h>
# include <iostream>
# include <unistd.h>
# include <vector>
# include <sstream>

# define BUFFER_SIZE 10000

std::vector<char> defaultPage(void) {
    std::vector<char> _body;
    std::stringstream sb;
    sb << "HTTP/1.1 200 OK\r\n";
    sb << "Content-Type: text/plain\r\n";
    sb << "Content-Length: 200\r\n\r\n";
	sb << "<html data-lt-installed=\"true\">";
	sb << "<head><title>";
	sb << "200 " << "OK";
	sb << "</title></head>";
	sb << "<body>";
	sb << "<center><h1>";
	sb << "200" << " " << "OK";
	sb << "</h1></center>";
	sb << "<hr><center>";
	sb << "RAVY" << "</center>";
	sb << "</body></html>\r\n";
    std::string page = sb.str();
    _body.insert(_body.end(), page.begin(), page.end());
    return _body;
}

int main() {
	int epollFD = epoll_create(1);
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
        std::cerr << "error 1" << std::endl; //gai_strerror(status)
    }

	// BIND DA FD DO SERVER COM A PORTA E HOST
	int optval = 1;
	int listener = 1;
	for (; result != NULL; result = result->ai_next) {
		listener = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
		if (listener == -1)
			continue;
		if (setsockopt(listener, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(int)) == -1) {
			std::cerr << "Error 2" << std::endl;
		}
		if (bind(listener, result->ai_addr, result->ai_addrlen) == 0) {
			break;
		}
		if (close(listener) == -1)
			std::cerr << "Error on close 3" << std::endl;
	}
	if (result == NULL)
		std::cerr << "Error 4" << std::endl;

	// COMEÇA A OUVIR NO HOST E PORTA
	if (listen(listener, 10) == -1) {
		std::cerr << "Error 5" << std::endl;
	}

	// ADICIONA O SERVER NO EPOLL
	event.events = EPOLLIN;
	event.data.fd = listener;
	if (epoll_ctl(epollFD, EPOLL_CTL_ADD, listener, &event) == -1) {
		std::cerr << "Error 6" << std::endl;
	}

	while (true) {
        // ESPERA NOVOS CLIENTES
		struct epoll_event  clientEvents[100];
		int number_of_ready_fds = epoll_wait(epollFD, clientEvents, 100, -1);
		if (number_of_ready_fds == -1) {
			std::cerr << "Error 7" << std::endl;
		}

		for (int i = 0; i < number_of_ready_fds; i++) {
			int new_socket = -1;
			socklen_t                   addrlen;
			struct sockaddr_storage     client_saddr;

            // HANDSHAKE
			addrlen = sizeof(struct sockaddr_storage);
            if (clientEvents[i].data.fd == listener) {
                if ((new_socket = accept(listener, (struct sockaddr *) &client_saddr, &addrlen)) < 0) {
                    std::cerr << "Error 8" << std::endl;
                }

                struct epoll_event event;
	            memset(&event, '\0', sizeof(struct epoll_event));
                event.events = EPOLLIN;
	            event.data.fd = new_socket;
                if (epoll_ctl(epollFD, EPOLL_CTL_ADD, new_socket, &event) == -1) {
                    std::cerr << "Error 11" << std::endl;
                }
                continue;
            }

            // LER DO CLIENTE E ADICIONAR FD COMO EPOLLOUT
            if (clientEvents[i].events & EPOLLIN) {
                char request[BUFFER_SIZE];
                memset(request, '\0', sizeof(char) * BUFFER_SIZE);
                recv(clientEvents[i].data.fd, request, sizeof(char) * BUFFER_SIZE, 0);
                printf("Requisição recebida:\n%s\n", request);

                struct epoll_event event;
	            memset(&event, '\0', sizeof(struct epoll_event));
                event.events = EPOLLOUT;
	            event.data.fd = clientEvents[i].data.fd;
                if (epoll_ctl(epollFD, EPOLL_CTL_MOD, clientEvents[i].data.fd, &event) == -1) {
                    std::cerr << "Error 10" << std::endl;
                }
            }

            // ESCREVO PARA O CLIENTE, DELETO FD DO EPOLL E FECHO O FD 
            if (clientEvents[i].events & EPOLLOUT) {
                std::vector<char> response = defaultPage();
                send(clientEvents[i].data.fd, &response[0], sizeof(char) * response.size(), 0);
                printf("Resposta enviada!\n");
			    
                if (epoll_ctl(epollFD, EPOLL_CTL_DEL, clientEvents[i].data.fd, NULL) == -1) {
                    std::cerr << "Error 10" << std::endl;
                }
                close(clientEvents[i].data.fd);
            }

		}
	}
}

