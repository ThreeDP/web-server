#ifndef __HTTP_HPP__
# define __HTTP_HPP__

# include "Server.hpp"
# include "Client.hpp"

# define BUFFER_SIZE 2048

class Http {
    // USED
    ILogger                             *_logger;
    std::map<std::string, IServer *>    servers;
    std::map<int, IServer *>            clientFD_Server;
    std::vector<IServer *>              _serversPointer;
    std::map<int, IServer *>            _serverFDToServer;
    std::map<int, std::vector<char> >          _clientFDToRequest;
    std::map<int, Client>               _clientFDToClient;

    // Parser Response
    std::map<int, int>            _cgis;

    struct addrinfo                         *_result;
    // epoll config
    //NOT USED
    std::map<int, std::string>          _serverFDToStringHost;

    public:
        // USED
        void Process(void);

        // Geters
        IServer                      *GetServer(std::string server);

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
        void SetServer(IServer *server);

    private:
        // Handler Epoll FDS
        void    CleanFds(int EpollFD);
        void    CheckTimeout(int EpollFD);
        void    CloseConnection(int EpollFD, int clientEvents_fd);
        void    AddConnection(int EpollFD, int client_fd);
        void    ModifyClientFDState(int EpollFD, int clientEvents_fd, uint32_t epollState);

        // Handler Requests and Responses
        std::vector<char>   ReadRequest(int EpollFD, struct epoll_event &clientEvent);
        bool                WriteResponse(int EpollFD, struct epoll_event &clientEvents);
        bool                CGIWriteRequest(int EpollFD, struct epoll_event &clientEvent);
        bool                CGIReadResponse(int EpollFD, struct epoll_event &clientEvent);
        bool                HandShake(int EpollFD, struct epoll_event &clientEvent);
        bool                ProcessResponse(int EpollFD, struct epoll_event &clientEvent, size_t numbytes, HttpRequest &Req, bool Continue);
};

#endif