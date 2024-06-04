#ifndef __SERVER_HPP__
# define __SERVER_HPP__

# include "HttpResponse.hpp"
# include "HttpRequest.hpp"
# include "CommonParameters.hpp"
# include <sys/types.h>


# include <iostream>

#include <fstream>

#include <string>
#include <iostream>
#include <sstream>

# include <set>
# include <vector>


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

class Server : public CommonParameters {
    private:
        std::string     _ip;
        std::string     _ipVersion;
        ServerStages    _stage;

        int             _actualClientFD;

    public:

        std::map<std::string, Route *>          routes;
        
        // socket config
        struct addrinfo                         hints;
        struct addrinfo                         *result;

        int                                     listener;
        const static int                        backlog = 10;

        std::map<int, HttpResponse>             ClientsResponse;

        Server(std::string name){
            this->_listen_host = "127.0.0.1";
            this->_listen_port = 8081;
            this->_server_names.push_back(name);
            // this->_default_error_page[404] = "/404.html";
            // this->_default_error_page[500] = "/50x.html";
            // this->_default_error_page[502] = "/50x.html";
            // this->_default_error_page[503] = "/50x.html";
            // this->_default_error_page[504] = "/50x.html";
            this->_limit_client_body_size = 2 * 1024;
            this->_root = "../home";
            this->_index.insert("index.html");
            this->_index.insert("new.html");
            this->_autoindex = false;
        }
    
    public:

        // Server Methods
        void                    SetAddrInfo(void);
        void                    CreateSocketAndBind(void);
        int                     StartListen(void);
        int                     AcceptClientConnect(void);

        // Server Process
        std::string             ProcessResponse(int client_fd);
        void                    ProcessRequest(std::string buffer, int client_fd);
        std::string             FindMatchRoute(HttpRequest &res);

        // Base Methods
        Server(void);
        ~Server(void);
        Server(std::string name);
        Server(std::string name, int port, std::string root);
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