#ifndef __HTTP_HPP__
# define __HTTP_HPP__

# include "Server.hpp"
# include <map>

class Http {
    std::map<std::string, Server *>  servers;
    
    public:
        Http(){};
        ~Http(){};

        Server *GetServer(std::string server);
        void SetServer(std::string serverName, Server *server);
};

#endif