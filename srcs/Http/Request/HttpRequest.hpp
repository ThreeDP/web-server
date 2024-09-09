#ifndef __HTTP_REQUEST_HPP__
# define __HTTP_REQUEST_HPP__

# include <map>
# include <iostream>
# include <sstream>
# include <string>
# include <cstring>
# include <vector>
# include "define.hpp"

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
        // AllowMethod:          GET
        std::map<std::string, std::string>  _payload;
        
        void                ParserRequest(std::string request);

        std::vector<std::string>             GetEnvp(void) {
            std::vector<std::string> vec;
            std::stringstream str(_queryStrings);
            std::string item;
            while (std::getline(str, item, '&')) {
                //std::cout << item << std::endl;
                vec.push_back(item);
            }

            std::map<std::string, std::string>::iterator it = _payload.begin();
            vec.push_back("REQUEST_METHOD=" + _method);
            for ( ;  it != _payload.end(); ++it) {
                std::string key = it->first.substr(0, it->first.size() - 1);
                //std::cout << it->first << "     " << it->second << std::endl;
                vec.push_back(key + "=" + it->second);
            }
            // std::string line;
            // std::stringstream ss(_body);
            // while (std::getline(ss, line, '&')) {
            //     vec.push_back(line);
            // }
            return vec;
        }

        void    ParserRequest(std::vector<char> request) {
            std::string str(request.begin(), request.end());

            size_t firstHeaderEnd = str.find("\r\n\r\n") + 4;
            if (firstHeaderEnd == std::string::npos) {
                std::cerr << "Formato de solicitação HTTP inválido: cabeçalhos não encontrados" << std::endl;
                return;
            }

            size_t secondHeaderStart = str.find("\r\n\r\n", firstHeaderEnd);
            if (secondHeaderStart == std::string::npos) {
                secondHeaderStart = str.size();
            }

            std::string firstHeaders = str.substr(0, firstHeaderEnd);

            std::string secondHeaders;
            if (secondHeaderStart > firstHeaderEnd + 4) {
                secondHeaders = str.substr(firstHeaderEnd + 4, secondHeaderStart - (firstHeaderEnd + 4));
                firstHeaderEnd = secondHeaderStart + 4;
            }

            _bodyBinary.clear();
            _bodyBinary.assign(request.begin() + firstHeaderEnd, request.end());
            // Parse request line
            std::stringstream srequest(str.substr(0, firstHeaderEnd));
            std::string line;

            // Read the request line
            if (std::getline(srequest, line)) {
                std::stringstream requestLine(line);
                std::getline(requestLine, _method, ' ');
                std::getline(requestLine, _path, ' ');
                std::getline(requestLine, _HTTPVersion, '\r');
                
                // Handle query strings
                size_t pos = _path.find_first_of('?');
                if (pos != std::string::npos) {
                    _queryStrings = _path.substr(pos + 1);
                    _path = _path.substr(0, pos);
                }
            }

            // Parse headers
            std::stringstream headerStream(str);
            while (std::getline(headerStream, line) && !line.empty()) {
                std::string key;
                std::string value;
                std::stringstream headerLine(line);
                if (std::getline(std::getline(headerLine, key, ':'), value)) {
                    // Trim leading spaces from the value
                    value.erase(0, value.find_first_not_of(" \t"));
                    _payload[key + ":"] = value;
                }
            }
            // Extract body
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
        std::string                     GetQueryParams(void) const {
            return _queryStrings;
        }

        bool                          IsCGIRequest(void);

        bool operator==(const HttpRequest &other);
};

std::ostream &operator<<(std::ostream &os, HttpRequest const &route);

#endif