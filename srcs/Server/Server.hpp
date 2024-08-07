#ifndef __SERVER_HPP__
# define __SERVER_HPP__

# include "Route.hpp"
#include <netdb.h>
#include <sys/epoll.h>
# include "Response201Created.hpp"
# include "Response404NotFound.hpp"

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
        IHandler        *_handler;

        int             _actualClientFD;

    public:

        std::map<std::string, Route *>          routes;
        
        // socket config
        struct addrinfo                         hints;
        struct addrinfo                         *result;

        int                                     listener;
        const static int                        backlog = 10;

        std::map<int, RouteResponse *>             ClientsResponse;

    public:

        // Server Methods
        std::string GenerateAutoindex(std::vector<struct dirent *> *dirs, std::string path);
        void                    SetAddrInfo(void);
        void                    CreateSocketAndBind(void);
        int                     StartListen(void);
        int                     AcceptClientConnect(void);

        // Server Process

        std::string             ProcessResponse(int client_fd);
        void                    ProcessRequest(HttpRequest &request, int client_fd);
        void                    ProcessRequest(std::string buffer, int client_fd);
        std::string             FindMatchRoute(HttpRequest &res);

        // Geters

        int                     GetListener(void) const;
        std::string             GetHosts(void) const;
        std::string             GetIP(void) const;
        std::string             GetIPVersion(void) const;
        std::string             GetListenPort(void) const;
        ServerStages            GetStage(void) const;
        int                     GetClientFD(void) const;

        // Seters

    private:
        void            _setServerIpInfos(struct addrinfo *result);
    
    public:
        void            UpdateState(ServerStages st, int client_fd);

        // Base Methods

        Server(void);
        ~Server(void);
        Server(std::string name);
        Server(std::string name, IHandler *handler);
        Server(std::string name, int port, std::string root);
        Server(std::vector<std::string> serv, unsigned short port);
        
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