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

#include <fstream>
# include <dirent.h>

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

        Route() : _directory("."){}

        std::string     IsADirectory(std::string path) {
            DIR *dir = NULL;
    
            dir = opendir(path.c_str());
            if (dir != NULL) {
                struct dirent* entry;

                while ((entry = readdir(dir)) != NULL) {
                    std::string d_name = entry->d_name; 
                    if (d_name == "index.html") {
                        path += "/" + d_name;
                    }
                }
                closedir(dir);
            }
            return path;
        }

        std::string    ProcessRoute(std::string path) {
            std::string body;
            std::string newP = this->_directory;
            newP += path;

            newP = this->IsADirectory(newP);
            std::ifstream file(newP.c_str());
            if (file.is_open()) {
                std::string line;
                while (std::getline(file, line)) {
                    body += line;
                }
                file.close();
            }
            return body;
        }
        //Route(std::vector<std::string> methods, std::string redirect, std::string directory);
};

#endif