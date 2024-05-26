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

class Server {
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
        
        // Geters
        int             GetListener(void) const;

        // Seters

        // Server Methods
        void            SetAddrInfo(void);
        void            CreateSocketAndBind(void);
        int             StartListen(void);
        
        // Base Methods
        Server(void);
        ~Server(void);
        Server(std::vector<std::string> serv, unsigned short port) : server_names(serv), listen_port(port) {
            memset(&hints, 0, sizeof(struct addrinfo));
            this->hints.ai_family = AF_UNSPEC;
            this->hints.ai_socktype = SOCK_STREAM;
            this->hints.ai_flags = AI_PASSIVE;

            this->result = NULL;
        }

        std::string ProcessResponse(int client_fd) {
            // HttpResponse send_msg(
            //     "<!DOCTYPE html><html><body><h1>Hello beaaa!</h1></body></html>",
            //     "200",
            //     "text/html"
            // );
            std::string response = this->ClientsResponse[client_fd].CreateResponse();
            this->ClientsResponse.erase(client_fd);
            return response;
        }

        void     ProcessRequest(std::string buffer, int client_fd) {
            HttpRequest     res;
            DIR             *dir = NULL;
            res.ParserRequest(buffer);
            std::string     path = std::string("." + res._path);

            dir = opendir(path.c_str());
            if (dir != NULL) {
                struct dirent* entry;

                while ((entry = readdir(dir)) != NULL) {
                    std::string d_name = entry->d_name; 
                    if (d_name == "index.html") {
                        path += "/" + d_name;
                    }
                    std::cout << path << std::endl;
                }
                closedir(dir);
            }
            std::string body;
            std::ifstream file(path.c_str());
            if (file.is_open()) {
                std::string line;
                while (std::getline(file, line)) {
                    body += line;
                }
                file.close();
            }
            this->ClientsResponse[client_fd] = HttpResponse(
                body,
                "200",
                "text/html"
            );
        }

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