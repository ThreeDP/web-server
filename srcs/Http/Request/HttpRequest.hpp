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
        size_t                              _bodySize;

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

        void    HttpRequest::ParserRequest(std::vector<char> request) {
            int                 i = 0;
            std::string         line;
            std::string         str(request.begin(), request.end());
            ssize_t             found = str.find("\r\n\r\n");

            std::stringstream   srequest(str.substr(0, found + 4));
            bool                isBody = false;

            while (std::getline(srequest, line)) {
                ++i;
                std::stringstream swords(line);
                if (i == 1) {
                    std::getline(swords, this->_method, ' ');
                    std::getline(swords, this->_path, ' ');
                    std::getline(swords, this->_HTTPVersion, '\r');
                    continue;
                }
                size_t pos = this->_path.find_first_of('?');
                if (pos != std::string::npos) {
                    _queryStrings = this->_path.substr(pos + 1, this->_path.size());
                    this->_path = this->_path.substr(0, pos);
                }
                // bool isSlashRFirst = std::strncmp(swords.str().c_str(), "\r", 2) == 0;
                // if (isBody || isSlashRFirst) {
                //     if (!isBody && isSlashRFirst) {
                //         isBody = true;
                //         continue;
                //     } else if (isBody && !isSlashRFirst) {
                //         std::string body;
                //         std::getline(swords, body, '\r');
                //         this->_body += body;
                //         this->_bodySize += this->_body.length();
                //         continue;
                //     } else {
                //         this->_bodySize += 2;
                //         break;
                //     }
                // }
                std::string key;
                std::string value;
                std::getline(swords, key, ' ');
                std::getline(swords, value, '\r');
                this->_payload[key] = value;
            }
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