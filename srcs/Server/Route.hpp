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
#include <ctime>
#include <stdint.h>
#include <set>
// #include <time.h>
# include "RouteResponse.hpp"
# include "HttpRequest.hpp"

enum RouteStages {
    R_START,
    R_REQUEST
};

class Route {
    private:
        std::string                                 _route_name;
        std::set<std::string>                       *_allow_methods;
        std::map<int, std::string>                  &_error_page;
        int                                         _limit_client_body_size;
        std::string                                 _redirectPath;
        std::string                                 _directory;
        bool                                        _autoIndex;
        std::set<std::string>                       &_index;
        
    public:
        RouteStages                                 _stage;

        // Route Methods
        std::set<std::string>       *CatDirectorysFiles(std::string path, std::vector<struct dirent *> &dirs);
        RouteResponse               *ProcessRoute(HttpRequest &httpReq);
        std::string                 ReturnFileRequest(std::string path);
        mode_t                      CatFileMode(std::string &path, int &statusCode);
        bool                        FindFilePattern(std::string &path, std::set<std::string> *dirs);
        RouteResponse               *DetermineOutputFile(HttpRequest &httpReq);
        std::string                 GenerateAutoindex(std::vector<struct dirent *> dirs, std::string path);
        RouteResponse               *checkFilePermission(HttpRequest &httpReq, int &statusCode);
        // Geters
        std::string                 GetRedirectPath(void);
        std::set<std::string>       *GetAllowMethods(void);
        void                        pathReset(std::string &path);
        std::string                 GetRouteName(void) const;
        // Seters
        void    SetAllowMethods(std::set<std::string> *methods);
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
        // Static functions
        static std::string          getActualDir(std::string path);
        static std::string	        getLastModifiedOfFile(const std::string &filename);
        static std::string          getFileSize(const std::string &filename);
        static time_t	            convertTimeToGMT(time_t t);
        static std::string	        formatTimeString(time_t	time);
        static std::string          getCurrentTimeInGMT(void);
        static void                 checkPathEnd(std::string &path, std::string append);
};

std::ostream &operator<<(std::ostream &os, Route const &route);

template<typename T>
std::string	toString(const T& value) {
	std::ostringstream oss;
	oss << value;
	return (oss.str());
}

#endif
