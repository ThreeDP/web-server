#ifndef __AHTTP_RESPONSE_HPP__
# define __AHTTP_RESPONSE_HPP__

# include "define.hpp"
# include <iostream>
# include <sstream>
# include <map>
# include <set>
# include "Utils.hpp"
# include "IHttpResponse.hpp"
# include "IBuilderResponse.hpp"

class HttpResponse : public IHttpResponse {

	protected:

		std::string                         _HTTPVersion;
        std::string                         _statusCode;
        std::string                         _statusMessage;
        std::map<std::string, std::string>  _headers;
        std::string                         _body;

        // Create Body Methods
        void                    _defaultErrorPage(void);
        void                    _createBodyByDirectory(std::vector<struct dirent*> *dirent);
        void                    _createBodyByFileDescriptor(std::ifstream *fd);

	public:

        // Create Response
        std::string             CreateResponse(void);
        std::string             ToString(void);

        // Geters
        std::string             GetStatusName(HttpStatusCode::Code statusCode);
        std::string             GetTextContent(std::string extension);
        std::string             GetBody(void) const;

        // Seters
        static void SetDefaultAHTTPResponse(void) {
            static  HttpResponse base = HttpResponse(0);
        }
        
        // Base Methods
        HttpResponse(void);
        ~HttpResponse(void) {}
    
    private:

        void            SetStatusCode(HttpStatusCode::Code statusCode);
        void            SetHeader(std::string key, std::string value);
        void            SetBody(std::string body);

        HttpResponse(int num);
        const std::string                    _server = "Ravy 1.0.0";
        static std::map<HttpStatusCode::Code, std::string> _mapStatusCode;
        static std::set<std::string>                _CGIExtensions;
        static std::map<std::string, std::string>   _mapTextContent;
        

        static void    _setMapTextContent(void) {
            _mapTextContent["text"] = "text/plain";
            _mapTextContent[".txt"] = "text/plain; charset=utf-8";
            _mapTextContent[".html"] = "text/html; charset=utf-8";
            _mapTextContent[".css"] = "text/css; charset=utf-8";
            _mapTextContent[".jpg"] = "image/jpg";
            _mapTextContent[".jpeg"] = "image/jpeg";
            _mapTextContent[".png"] = "image/png";
            _mapTextContent[".mp4"] = "video/mp4";
            _mapTextContent[".ico"] = "image/vnd.microsoft.icon";
            _mapTextContent[".php"] = "text/plain; charset=utf-8";
            _mapTextContent[".js"] = "application/javascript";
            _mapTextContent[".gif"] = "image/gif";
        }

        static void    _setMapStatusCode(void) {
            _mapStatusCode[HttpStatusCode::_OK] = "OK";
            _mapStatusCode[HttpStatusCode::_CREATED] = "Created";
            _mapStatusCode[HttpStatusCode::_ACCEPTED] = "Accepted";
            _mapStatusCode[HttpStatusCode::_NO_CONTENT] = "No Content";
            _mapStatusCode[HttpStatusCode::_MUILTIPLE_CHOICE] = "Multiple Choice";
            _mapStatusCode[HttpStatusCode::_MOVED_PERMANENTLY] = "Moved Permanently";
            _mapStatusCode[HttpStatusCode::_FOUND] = "Found";
            _mapStatusCode[HttpStatusCode::_PERMANENT_REDIRECT] = "Permanent Redirect";
            _mapStatusCode[HttpStatusCode::_BAD_REQUEST] = "Bad Request";
            _mapStatusCode[HttpStatusCode::_UNAUTHORIZED] = "Unauthorized";
            _mapStatusCode[HttpStatusCode::_FORBIDDEN] = "Forbidden";
            _mapStatusCode[HttpStatusCode::_NOT_FOUND] = "Not Found";
            _mapStatusCode[HttpStatusCode::_METHOD_NOT_ALLOWED] = "Method Not Allowed";
            _mapStatusCode[HttpStatusCode::_CONTENT_TOO_LARGE] = "Content Too Large";
            _mapStatusCode[HttpStatusCode::_UNSUPPORTED_MEDIA_TYPE] = "Unsupported Media Type";
            _mapStatusCode[HttpStatusCode::_INTERNAL_SERVER_ERROR] = "Internal Server Error";
            _mapStatusCode[HttpStatusCode::_BAD_REQUEST] = "Bad Gateway";
            _mapStatusCode[HttpStatusCode::_GATEWAY_TIMEOUT] = "Gateway Timeout";
            _mapStatusCode[HttpStatusCode::_HTTP_VERSION_NOT_SUPPORTED] = "HTTP Version Not Supported";
        }

        static void    _setCGIExtensions(void) {
            _CGIExtensions.insert(".php");
            _CGIExtensions.insert(".go");
            _CGIExtensions.insert(".py");
        }

    // friend class IBuilderResponse;
};

#endif