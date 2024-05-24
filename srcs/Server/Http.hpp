#ifndef __HTTP_HPP__
# define __HTTP_HPP__

# include "Server.hpp"
# include <map>
#include <sys/socket.h>
#include <iomanip>
#include <ctime>

#include <string>

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

typedef struct HttpResponse_s {
  char statusCode[22];
  char date[36];
  char contentType[26];
  char contentLegth[23];
  char body[66];
} HttpResponse;

class Http {
    // Parser Response
    std::map<std::string, Server *>  servers;

    const static int eventsLimit = 10;
    
    // epoll config
    int                     _epollFD;
    struct epoll_event      serverEvents[eventsLimit];
    struct epoll_event      clientEvents[eventsLimit];

    public:

        // Http Methods
        void                        StartPollList(void);
        void                        StartWatchSockets(void);
        void                        ClientHandShake(Server *server);


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