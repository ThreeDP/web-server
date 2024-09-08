#ifndef __HTTP_HPP__
# define __HTTP_HPP__

# include "Server.hpp"

# define BUFFER_SIZE 2048

class Http {
    // USED
    ILogger                             *_logger;
    std::map<std::string, IServer *>    servers;
    std::map<int, IServer *>            clientFD_Server;
    std::vector<IServer *>              _serversPointer;
    std::map<int, IServer *>            _serverFDToServer;
    std::map<int, HttpRequest>    _clientFDToRequest;

    // Parser Response
    std::map<int, int>            _cgis;

    
    // epoll config
    //NOT USED
    const static int                    eventsLimit = 10;
    struct epoll_event                  clientEvents[eventsLimit];
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

};




#endif