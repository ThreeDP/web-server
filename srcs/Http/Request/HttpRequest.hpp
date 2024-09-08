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
        std::string                         _queryStrings;
        std::string                         _HTTPVersion;
        std::string                         _body;
        std::vector<char>                   _bodyBinary;
        size_t                              _bodySize;
        bool                                _continue;

    public:
        // AllowMethod:          GET
        std::map<std::string, std::string>  _payload;
        
        void                ParserRequest(std::string request);
        std::vector<std::string>             GetEnvp(void) {
            std::vector<std::string> vec;
            std::stringstream str(_queryStrings);
            std::string item;
            while (std::getline(str, item, '&')) {
                std::cout << item << std::endl;
                vec.push_back(item);
            }

            std::map<std::string, std::string>::iterator it = _payload.begin();
            vec.push_back("REQUEST_METHOD=" + _method);
            for ( ;  it != _payload.end(); ++it) {
                std::string key = it->first.substr(0, it->first.size() - 1);
                std::cout << it->first << "     " << it->second << std::endl;
                vec.push_back(key + "=" + it->second);
            }
            std::string line;
            std::stringstream ss(_body);
            while (std::getline(ss, line, '&')) {
                vec.push_back(line);
            }
            return vec;
        }

        // void    HttpRequest::ParserRequest(std::vector<char> request) {
        //     int                 i = 0;
        //     std::string         line;
        //     std::string         str(request.begin(), request.end());
        //     ssize_t             found = str.find("\r\n\r\n") + 4;

        //     std::stringstream   srequest(str.substr(0, found));
        //     bool                isBody = false;

        //     std::stringstream swords(line);
        //     std::getline(swords, this->_method, ' ');
        //     std::getline(swords, this->_path, ' ');
        //     std::getline(swords, this->_HTTPVersion, '\r');
        //     size_t pos = this->_path.find_first_of('?');
        //     if (pos != std::string::npos) {
        //         _queryStrings = this->_path.substr(pos + 1, this->_path.size());
        //         this->_path = this->_path.substr(0, pos);
        //     }
        //     while (std::getline(srequest, line)) {
        //         std::string key;
        //         std::string value;
        //         std::getline(swords, key, ' ');
        //         std::getline(swords, value, '\r');
        //         this->_payload[key] = value;
        //     }
        //     _bodyBinary.push_back(_bodyBinary.end(), request.begin() + found, request.end());
        // }

        void AddBody(char *buffer, int size) {
            _bodyBinary.insert(_bodyBinary.end(), buffer, buffer + size);
        }

        void ParserRequest(char *buffer, int size) {
            std::vector<char> request(buffer, buffer + size);
            std::string str(request.begin(), request.end());

            // Find the end of the headers section
            size_t found = str.find("\r\n\r\n");
            // if (found == std::string::npos) {
            //      // Handle error: headers section not found
            //      return;
            // }
            found += 4; // Move past "\r\n\r\n"

            // Extract headers part
            std::string headers = str.substr(0, found);

            // Parse request line
            std::stringstream srequest(str.substr(0, headers.length()));
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
            std::stringstream headerStream(headers);
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
            _bodyBinary.assign(request.begin() + found, request.end());
            _continue = (_payload.find("Expect:") != _payload.end() ? true : false);
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