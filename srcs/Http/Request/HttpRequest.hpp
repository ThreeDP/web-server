#ifndef __HTTP_REQUEST_HPP__
# define __HTTP_REQUEST_HPP__

# include <map>
# include <iostream>
# include <sstream>
# include <string>
# include <cstring>
# include <vector>

class HttpRequest {
    public:
        std::string                         _method;
        std::string                         _path;
        std::string                         _HTTPVersion;
        std::string                         _body;
        size_t                              _bodySize;

    public:
        // AllowMethod:          GET
        std::map<std::string, std::string>  _payload;
        
        void                ParserRequest(std::string request);
        std::vector<std::string>             GetEnvp(void) {
            std::vector<std::string> vec;
            std::map<std::string, std::string>::iterator it = _payload.begin();
            vec.push_back("REQUEST_METHOD=" + _method);
            for ( ;  it != _payload.end(); ++it) {
                std::string key = it->first.substr(0, it->first.size() - 1);
                vec.push_back(key + "=" + it->second);
            }
            std::string line;
            std::stringstream ss(_body);
            while (std::getline(ss, line, '\n')) {
                vec.push_back(line);
            }
            return vec;
        }

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

        bool operator==(const HttpRequest &other);
};

std::ostream &operator<<(std::ostream &os, HttpRequest const &route);

#endif