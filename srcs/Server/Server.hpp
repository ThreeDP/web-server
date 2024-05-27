#ifndef __SERVER_HPP__
# define __SERVER_HPP__

# include "HttpResponse.hpp"
# include "HttpRequest.hpp"
 # include <sys/types.h>
       # include <dirent.h>

# include <iostream>
# include <vector>
#include <fstream>

#include <string>
#include <iostream>
#include <sstream>


# include "Route.hpp"

enum ServerStages {
    S_START,
    S_LISTEN,
    S_CLIENT_CONNECT,
    S_CLIENT_REQUEST,
    S_SERVER_RESPONSE,
    S_CLIENT_DISCONNECT,
    S_END
};

class Server {
    private:
        std::string     _ip;
        std::string     _ipVersion;
        ServerStages    _stage;

        int             _actualClientFD;

    public:

        // Parser Result
        std::vector<std::string>        server_names;
        unsigned short                  listen_port;
        int                             limit_client_body_size;
        std::map<int, std::string>      default_error_page;
        
        // socket config
        struct addrinfo                 hints;
        struct addrinfo                 *result;

        int                             listener;
        const static int                backlog = 10;

        std::map<int, HttpResponse>  ClientsResponse;
    
    public:
        std::map<std::string, Route *>  routes;

        // Server Methods
        void                    SetAddrInfo(void);
        void                    CreateSocketAndBind(void);
        int                     StartListen(void);
        int                     AcceptClientConnect(void);

        // Server Process
        std::string             ProcessResponse(int client_fd);
        void                    ProcessRequest(std::string buffer, int client_fd);

        // Base Methods
        Server(void);
        ~Server(void);
        Server(std::vector<std::string> serv, unsigned short port);

        // Geters
        int             GetListener(void) const;
        std::string     GetHosts(void) const;
        std::string     GetIP(void) const;
        std::string     GetIPVersion(void) const;
        std::string     GetListenPort(void) const;
        ServerStages    GetStage(void) const;
        int             GetClientFD(void) const;

        // Seters
    private:
        void            _setServerIpInfos(struct addrinfo *result);
    
    public:
        void            UpdateState(ServerStages st, int client_fd);
        class Except: virtual public std::exception {
			protected:
			std::string error_message;
			public:
				explicit Except(const std::string& msg) : error_message(msg) {}
				virtual ~Except() throw () {}
				virtual const char* what() const throw () { return error_message.c_str();}
		};
};

std::ostream &operator<<(std::ostream &os, Server const &server);

#endif