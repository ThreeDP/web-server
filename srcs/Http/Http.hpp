#ifndef __HTTP_HPP__
# define __HTTP_HPP__

# include "Server.hpp"

enum HttpStages {
    H_START,
    H_CONFIG,
    H_ADD_SERVERS,
    H_STAND_BY,
    H_END
};

class Http {
    HttpStages _stage;
    ILogger *_logger;

    // Patterns HTTP

    // Parser Response
    std::map<std::string, IServer *>  servers;
    std::map<int, IServer *> clientFD_Server;

    const static int eventsLimit = 10;
    
    // epoll config
    int                     _epollFD;
    struct epoll_event      clientEvents[eventsLimit];

    public:

        // Http Methods
        void                        StartPollList(void);
        void                        StartWatchSockets(void);
        void                        ClientHandShake(IServer *server);
        void                        HandleResponse(int client_fd);
        bool                        ConnectClientToServer(int i);
        void                        DisconnectClientToServer(int client_fd);
        ssize_t                     HandleRequest(int client_fd);

        

        // Geters
        IServer                      *GetServer(std::string server);
        int                         &GetEPollFD(void);
        HttpStages                  GetStage(void) const;

        // Base Methods
        Http(ILogger *logger);
        ~Http(void);

        class Except: virtual public std::exception {
			protected:
			std::string error_message;
			public:
				explicit Except(const std::string& msg) : error_message(msg) {}
				virtual ~Except() throw () {}
				virtual const char* what() const throw () { return error_message.c_str();}
		};

        // Seters
        void SetServer(std::string serverName, IServer *server);

    private:

};

std::ostream &operator<<(std::ostream &os, Http const &http);



#endif