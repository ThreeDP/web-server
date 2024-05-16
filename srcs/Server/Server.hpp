#ifndef __SERVER_HPP__
# define __SERVER_HPP__

# include <iostream>
# include <vector>
# include "Route.hpp"
# include <map>


class Server {
    private:
        std::vector<std::string>        server_names;
        unsigned short                  listen;
        int                             limit_client_body_size;
        std::map<int, std::string>      default_error_page;
        
    
    public:
        std::map<std::string, Route *>  routes;
        Server(void){}
        ~Server(void) {}
        Server(std::vector<std::string> serv, unsigned short port);
};

#endif