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
# include "EHttpStatusCode.hpp"

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
        HttpStatusCode::Code                _flag;
        char **             GetEnvp(std::string saveon) {
            std::vector<std::string> ev;
            std::stringstream str(_queryStrings);
            std::string item;

            while (std::getline(str, item, '&')) {
                ev.push_back(item);
            }

            std::map<std::string, std::string>::iterator it = _payload.begin();
            ev.push_back("REQUEST_METHOD=" + _method);
            ev.push_back("QUERY_STRING=" + _queryStrings);
            size_t fileStart = _path.find_last_of('/');
            ev.push_back("SCRIPT_FILE=" + _path.substr(fileStart + 1, _path.size()));
            ev.push_back("SAVE_INTO=" + saveon);
            std::map<std::string, std::string>::iterator itp = _payload.find("Content-Disposition:");
            if (itp != _payload.end()) {
                std::stringstream ss(itp->second);
                std::string line;
                while (ss >> line) {
                    size_t varEnd = line.find('=');
                    size_t start = line.find_first_of("\"");
                    size_t end = line.find_last_of("\"");

                    if (varEnd != std::string::npos && start < end && start != std::string::npos && end != std::string::npos) {
                        ev.push_back(line.substr(0, varEnd) + "=" + line.substr(start + 1, end - start - 1));
                    }
                }
            }
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
        ssize_t                              GetBodySize(void) const;
        std::string                     GetQueryParams(void) const {
            return _queryStrings;
        }

        bool                          IsCGIRequest(void);

        bool operator==(const HttpRequest &other);
};

std::ostream &operator<<(std::ostream &os, HttpRequest const &route);

#endif