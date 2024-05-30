#ifndef __ROUTE_HPP__
# define __ROUTE_HPP__

# include "define.hpp"
# include "CommonParameters.hpp"

// I/O
# include <iostream>

// Containers
# include <vector>
# include <map>

// epoll
# include <sys/epoll.h>
# include <sys/stat.h>

// socket
# include <sys/types.h>
# include <sys/socket.h>

#include <fstream>
# include <dirent.h>

# include <vector>
# include <sstream>  

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
#include <set>
// #include <time.h>

class Route {
    private:
        std::set<std::string>                       &_allow_methods;
        std::map<int, std::string>                  &_error_page;
        int                                         _limit_client_body_size;
        std::string                                 _redirectPath;
        std::string                                 _directory;
        bool                                        _autoIndex;
        std::set<std::string>                       &_index;
        
    public:

        // Route Methods
        std::vector<std::string>    *CatDirectorysFiles(std::string path);
        std::string                 ProcessRoute(std::string path);
        std::string                 ReturnFileRequest(std::string path);
        mode_t                      CatFileMode(std::string path);
        bool                        FindFilePattern(std::string &path, std::vector<std::string> *dirs);
        std::string                 DetermineOutputFile(std::string path);

        // Geters
        std::string GetRedirectPath(void);
        std::set<std::string>    &GetAllowMethods(void);

        // Seters
        void    SetAllowMethods(std::set<std::string> &methods);
        void    SetRedirectPath(std::string redirect);

        // Base methods
        Route(CommonParameters *server, std::string server_name);

        class Except: virtual public std::exception {
			protected:
			std::string error_message;
			public:
				explicit Except(const std::string& msg) : error_message(msg) {}
				virtual ~Except() throw () {}
				virtual const char* what() const throw () { return error_message.c_str();}
		};
        //Route(std::vector<std::string> methods, std::string redirect, std::string directory);
};

#endif
