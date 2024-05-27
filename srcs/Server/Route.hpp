#ifndef __ROUTE_HPP__
# define __ROUTE_HPP__

# include "define.hpp"

// I/O
# include <iostream>

// Containers
# include <vector>
# include <map>

// epoll
# include <sys/epoll.h>

// socket
# include <sys/types.h>
# include <sys/socket.h>

# include <cstdlib>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <syslog.h>
#include <unistd.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdint.h>
// #include <time.h>

class Route {
    private:
        std::vector<std::string>            _allowMethods;
        std::map<std::string, std::string>  _redirectPath;
        std::string                         _directory;
        
    public:
        std::map<std::string, std::string> &GetRedirectPath(void);
        std::vector<std::string>    &GetAllowMethods(void);
        void    SetAllowMethods(std::vector<std::string> methods);
        void    SetRedirectPath(std::pair<std::string, std::string> redirect);

        Route(){}
        //Route(std::vector<std::string> methods, std::string redirect, std::string directory);
};

#endif