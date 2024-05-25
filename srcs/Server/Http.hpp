#ifndef __HTTP_HPP__
# define __HTTP_HPP__

# include "Server.hpp"

typedef struct HttpRequest_s {
    char        method[5];
    char        target[32];
    // std::string httpVersion;
    // std::map<std::string, std::string> headers; // Key-value pairs for header fields

    // // Optional fields (depending on your specific needs)
    // std::string referrer;
    // std::string ifModifiedSince;
    // std::string ifNoneMatch;
    // std::string cacheControl;
} HttpRequest_t;

class Http {
    // Parser Response
    std::map<std::string, Server *>  servers;
    std::map<int, Server *> clientFD_Server;

    const static int eventsLimit = 10;
    
    // epoll config
    int                     _epollFD;
    struct epoll_event      clientEvents[eventsLimit];

    public:

        // Http Methods
        void                        StartPollList(void);
        void                        StartWatchSockets(void);
        void                        ClientHandShake(Server *server);
        void                        HandleResponse(int client_fd);
        bool                        ConnectClientToServer(int i);
        void                        DisconnectClientToServer(int client_fd);
        ssize_t                     HandleRequest(int client_fd);

        // Seter
        void SetServer(std::string serverName, Server *server);

        // Geters
        Server                      *GetServer(std::string server);
        int                         &GetEPollFD(void);

        // Base Methods
        Http(void);
        ~Http(void);

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