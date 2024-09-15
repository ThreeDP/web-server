#ifndef __HTTP_REQUEST_HPP__
# define __HTTP_REQUEST_HPP__

# include <map>
# include <iostream>
# include <sstream>
# include <string>
# include <cstring>
# include <vector>
# include "define.hpp"
# include "IClient.hpp"

class HttpRequest {
    public:
        std::string                         _method;
        std::string                         _path;
        std::string                         _queryStrings;
        std::string                         _HTTPVersion;
        std::string                         _body;
        size_t                              _bodySize;
        std::vector<char>                   _bodyBinary;

    public:
        IClient *client;
        // AllowMethod:          GET
        std::map<std::string, std::string>  _payload;
        
        void                ParserRequest(std::string request);

        char **             GetEnvp(std::string saveon) {
            std::vector<std::string> ev;
            std::stringstream str(_queryStrings);
            std::string item;

            while (std::getline(str, item, '&')) {
                ev.push_back(item);
            }

            std::map<std::string, std::string>::iterator it = _payload.begin();
            ev.push_back("REQUEST_METHOD=" + _method);
            size_t fileStart = _path.find_last_of('/');
            std::cout << "Sring dentro de script file: " << _path.substr(fileStart, _path.size()) << std::endl;
            ev.push_back("SCRIPT_FILE=" + _path.substr(fileStart, _path.size()));
            ev.push_back("SAVE_INTO=" + saveon);
            for ( ;  it != _payload.end(); ++it) {
                std::string key = it->first.substr(0, it->first.size() - 1);
                ev.push_back(key + "=" + it->second);
            }
            char **envp = new char*[ev.size() + 1];
            for (size_t i = 0; i < ev.size(); ++i) {
                envp[i] = new char[ev[i].size() + 1];
                std::strcpy(envp[i], ev[i].c_str());
            }
            envp[ev.size()] = NULL;
            return envp;
        }

        void    ParserRequest(std::vector<char> &request);

        // Base Methods
        HttpRequest();
        ~HttpRequest();

        // Geters
        const std::map<std::string, std::string>  GetHeaders(void) const;
        std::string                         GetMethod(void) const;
        std::string                         GetPath(void) const;
        std::string                         GetHTTPVersion(void) const;
        std::string                         GetBody(void) const;
        int                              GetBodySize(void) const;
        std::string                     GetQueryParams(void) const {
            return _queryStrings;
        }

        bool                          IsCGIRequest(void);

        bool operator==(const HttpRequest &other);
};

std::ostream &operator<<(std::ostream &os, HttpRequest const &route);

#endif