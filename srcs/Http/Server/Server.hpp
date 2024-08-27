#ifndef __SERVER_HPP__
# define __SERVER_HPP__

# include "Route.hpp"
# include <netdb.h>
# include <sys/epoll.h>
# include "BuilderResponse.hpp"
# include "BuilderRoute.hpp"

enum ServerStages {
    S_START,
    S_LISTEN,
    S_CLIENT_CONNECT,
    S_CLIENT_REQUEST,
    S_SERVER_RESPONSE,
    S_CLIENT_DISCONNECT,
    S_END
};

class Server : public IServer {
    private:
        // Configs
        std::vector<std::string>                        _hosts;
        std::string                                     _port;
        std::set<std::string>                           _allowMethods;
        std::map<HttpStatusCode::Code, std::string>     _errorPages;
        int                                             _limit_client_body_size;
        std::map<std::string, std::string>              _redirectionPaths;
        std::string                                     _root;
        bool                                            _autoIndex;
        std::vector<std::string>                        _indexes;


        std::string     _ip;
        std::string     _ipVersion;
        IHandler        *_handler;
        ILogger         *_logger;

        int             _actualClientFD;

        // Seters
        void                        SetAllowMethods(std::set<std::string> methods);
        void                        SetErrorPage(std::set<HttpStatusCode::Code> statusCodes, std::string filePath);
        void                        SetBodyLimit(int size);
        void                        SetRedirectPath(std::pair<std::string, std::string> pair);
        void                        SetRootDirectory(std::string root);
        void                        SetPagesIndexes(std::vector<std::string> indexes);
        void                        SetAutoIndex(bool flag);
        void                        SetHosts(std::vector<std::string> hosts);
        void                        SetPort(std::string port);
        void                        SetRoute(std::string routeName, IRoute *route);

        std::string                 _toString(void);

    public:
        std::map<std::string, IRoute *>                 _routes;
        // Geters
        std::map<HttpStatusCode::Code, std::string>
                                    GetErrorPages(void);
        std::set<std::string>       GetAllowMethods(void);
        int                         GetBodyLimit(void);
        std::string                 GetRedirectPath(std::string path);
        std::string                 GetRootDirectory(void);
        std::vector<std::string>    GetPageIndexes(void);
        bool                        GetAutoIndex(void);
        std::vector<std::string>    GetHosts(void);
        IRoute                      *GetRoute(std::string routeName);

        // socket config
        struct addrinfo                         hints;
        struct addrinfo                         *result;

        int                                     listener;
        const static int                        backlog = 10;

        std::map<int, IHttpResponse *>        ResponsesMap;

    public:

        std::string                 GetPort(void);
        // Server Methods
        void                    SetAddrInfo(void);
        void                    CreateSocketAndBind(void);
        int                     StartListen(void);
        int                     AcceptClientConnect(void);

        // Server Process

        IHttpResponse*             ProcessResponse(int client_fd);
        void                    ProcessRequest(HttpRequest &request, int client_fd);
        std::string             FindMatchRoute(HttpRequest &res);

        // Geters

        int                     GetListener(void) const;
        std::string             GetHosts(void) const;
        std::string             GetIP(void) const;
        std::string             GetIPVersion(void) const;
        std::string             GetListenPort(void) const;

        // Seters

    private:
        void            _setServerIpInfos(struct addrinfo *result);
    
    public:
        void            UpdateState(ServerStages st, int client_fd);

        // Base Methods

        Server(IHandler *handler, ILogger *logger);
        ~Server(void);
        
        class Except: virtual public std::exception {
			protected:
			std::string error_message;
			public:
				explicit Except(const std::string& msg) : error_message(msg) {}
				virtual ~Except() throw () {}
				virtual const char* what() const throw () { return error_message.c_str();}
		};
};

#endif