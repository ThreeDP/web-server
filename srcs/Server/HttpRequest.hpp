#ifndef __HTTP_REQUEST_HPP__
# define __HTTP_REQUEST_HPP__

# include <map>
# include <iostream>
# include <sstream>

class HttpRequest {
    public:
    std::string                         _method;
    std::string                         _path;
    std::string                         _HTTPVersion;
    std::map<std::string, std::string>  _payload;
    std::string                         _body;

    public:
        HttpRequest(){}

        void    ParserRequest(std::string request) {
            static int          i = 0;
            //static int          size = 0;
            std::stringstream   srequest(request);
            std::string          line;

            while (std::getline(srequest, line)) {
                ++i;
                std::stringstream swords(line);
                if (i == 1) {
                    std::getline(swords, this->_method, ' ');
                    std::getline(swords, this->_path, ' ');
                    std::getline(swords, this->_HTTPVersion, ' ');
                    std::cout << "linha ( " << i << " ) " << this->_method << " " << this->_path << " " << this->_HTTPVersion << std::endl;
                    continue;
                }
                std::string key;
                std::string value;
                std::getline(swords, key, ' ');
                std::getline(swords, value, '\r');
                this->_payload[key] = value;
                std::cout << "linha ( " << i << " ) '" << key << " " << value << "'" << std::endl;
            }
        }
};

#endif