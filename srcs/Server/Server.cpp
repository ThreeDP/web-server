# include "Server.hpp"

Server::Server(std::vector<std::string> serv, unsigned short port): server_names(serv), listen(port), limit_client_body_size(1048576)
{
    std::cout << "Start a New Server!" << std::endl;
}