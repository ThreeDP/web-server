#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include "define.hpp"

#include <cstring>
#include <iostream>
#include <string>

int main(int ac, char **av) {
	if (ac != 3) {
		std::cerr << "Pass the ipaddr and port" << std::endl;
		return -1;
	}

	addrinfo hints, *p;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	int gAddRes = getaddrinfo(av[1], av[2], &hints, &p);
	if (gAddRes != 0) {
		std::cerr << gai_strerror(gAddRes) << std::endl;
		return -2;
	}
	if (p == NULL) {
		std::cerr << "No addr found" << std::endl;
	}

	int sockFD = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
	if (sockFD == -1) {
		std::cerr << "Erro while creating socket" << std::endl;
		return -4;
	}

	int connectR = connect(sockFD, p->ai_addr, p->ai_addrlen);
	if (connectR == -1) {
		close(sockFD);
		std::cerr << "Error while connecting socket" << std::endl;
		return -5;
	}

	std::string reply(15, ' ');

	ssize_t bytes_recv = recv(sockFD, &reply, reply.size(), 0);
	if (bytes_recv == -1) {
		std::cerr << "Error while receiving bytes" << std::endl;
		return -6;
	}
	std::cout << "Client recieved " << reply << std::endl;
	close(sockFD);
	freeaddrinfo(p);
	return 0;
}