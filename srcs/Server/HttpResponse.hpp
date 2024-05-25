#ifndef __HTTP_RESPONSE_HPP__
# define __HTTP_RESPONSE_HPP__

# include <iomanip>
# include <ctime>
# include <map>
# include <cstring>
# include <sstream> 

class HttpResponse {
    std::string                         _HTTPVersion;
    std::string                         _StatusCode;
    std::string                         _Status;
    std::map<std::string, std::string>  _payload;
    std::string                         _Body;

    HttpResponse(){}

    public:
        ~HttpResponse(){}
        HttpResponse(std::string body, std::string statusCode, std::string contentType) {
            this->_HTTPVersion = "HTTP/1.1";
            this->_StatusCode = statusCode;
            this->_Status = "OK";
            this->_payload["Content-Type:"] = contentType;
            std::stringstream num;
            num << body.size() + 2;
            this->_payload["Content-Length:"] = std::string(num.str());
            this->_payload["Date:"] = this->GetTimeNow();
            this->_Body = body;
        }

        std::string CreateResponse(void) {
            std::stringstream response;

            response << this->_HTTPVersion << " " << this->_StatusCode << " " << this->_Status << "\r\n";
            std::map<std::string, std::string>::iterator it = this->_payload.begin();
            for (; it != this->_payload.end(); ++it) {
                response << it->first << " " << it->second << "\r\n";
            }
            response << "\r\n" << this->_Body << "\r\n";
            return response.str();
        }

        std::string GetTimeNow(void) {
            char buffer[80];
            memset(&buffer, '\0', sizeof(char) * 80);
            std::time_t now = std::time(NULL);
            std::tm* gmt_time = std::gmtime(&now);
            std::stringstream ss;

            std::strftime(buffer, sizeof(char) * 80, "%Y-%m-%d %H:%M:%S GMT", gmt_time);
            ss << buffer;
            return ss.str();
        }
};

#endif