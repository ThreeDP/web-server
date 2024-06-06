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

    // Patterns HTTP
    static std::map<std::string, std::string>  _mapStatusCode;
    static std::set<std::string>               _CGIExtensions;
    static std::map<std::string, std::string>  _mapTextContent;

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

        

        // Geters
        Server                      *GetServer(std::string server);
        int                         &GetEPollFD(void);
        HttpStages                  GetStage(void) const;

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

        // Seters
        void SetServer(std::string serverName, Server *server);

    private:
        void    _setMapTextContent(void) {
            _mapTextContent["text"] = "Content-Type: text/plain";
            _mapTextContent[".txt"] = "Content-Type: text/plain; charset=utf-8";
            _mapTextContent[".html"] = "Content-Type: text/html; charset=utf-8";
            _mapTextContent[".css"] = "Content-Type: text/css; charset=utf-8";
            _mapTextContent[".jpg"] = "Content-type: image/jpg";
            _mapTextContent[".jpeg"] = "Content-type: image/jpeg";
            _mapTextContent[".png"] = "Content-type: image/png";
            _mapTextContent[".mp4"] = "Content-type: video/mp4";
            _mapTextContent[".ico"] = "Content-type: image/vnd.microsoft.icon";
            _mapTextContent[".php"] = "Content-Type: text/plain; charset=utf-8";
            _mapTextContent[".js"] = "Content-Type: application/javascript";
            _mapTextContent[".gif"] = "Content-Type: image/gif";
        }

        void    _setMapStatusCode(void) {
            _mapStatusCode["200"] = "OK";
            _mapStatusCode["201"] = "Created";
            _mapStatusCode["202"] = "Accepted";
            _mapStatusCode["204"] = "No Content";
            _mapStatusCode["300"] = "Multiple Choice";
            _mapStatusCode["301"] = "Moved Permanently";
            _mapStatusCode["302"] = "Found";
            _mapStatusCode["400"] = "Bad Request";
            _mapStatusCode["401"] = "Unauthorized";
            _mapStatusCode["403"] = "Forbidden";
            _mapStatusCode["404"] = "Not Found";
            _mapStatusCode["405"] = "Method Not Allowed";
            _mapStatusCode["413"] = "Request Entity Too Large";
            _mapStatusCode["415"] = "Unsupported Media Type";
            _mapStatusCode["500"] = "Internal Server Error";
            _mapStatusCode["502"] = "Bad Gateway";
            _mapStatusCode["504"] = "Gateway Timeout";
            _mapStatusCode["505"] = "HTTP Version Not Supported";
        }

        void    _setCGIExtensions(void) {
            _CGIExtensions.insert(".php");
            _CGIExtensions.insert(".go");
            _CGIExtensions.insert(".py");
        }
};

std::ostream &operator<<(std::ostream &os, Http const &http);



#endif