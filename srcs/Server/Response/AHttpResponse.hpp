#ifndef __AHTTP_RESPONSE_HPP__
# define __AHTTP_RESPONSE_HPP__

# include "define.hpp"
# include <iostream>
# include <sstream>
# include <map>
# include <set>
# include "Utils.hpp"

// # include "RouteResponse.hpp"

class AHttpResponse {

	protected:

		std::string                         _HTTPVersion;
        std::string                         _statusCode;
        std::string                         _status;
        std::map<std::string, std::string>  _payload;
        std::string                         _body;

        std::string             _defaultErrorPage(void);

	public:

        // Create Response
        std::string             CreateResponse(void);

        // Geters
        std::string             GetStatusName(std::string statusCode);
        std::string             GetTextContent(std::string extension);

        // Seters
        static void                    SetDefaultAHTTPResponse(void);
        
        // Base Methods

        AHttpResponse(std::string statusCode, std::string extension, std::string body);
        AHttpResponse(std::string statusCode, std::string extension);
        virtual ~AHttpResponse();
    private:

        static std::map<std::string, std::string>  _mapStatusCode;
        static std::set<std::string>               _CGIExtensions;
        static std::map<std::string, std::string>  _mapTextContent;
        
        AHttpResponse(void);

        void    _setMapTextContent(void) {
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

        void    _setMapStatusCode(void) {
            _mapStatusCode["200"] = "OK";
            _mapStatusCode["201"] = "Created";
            _mapStatusCode["202"] = "Accepted";
            _mapStatusCode["204"] = "No Content";
            _mapStatusCode["300"] = "Multiple Choice";
            _mapStatusCode["301"] = "Moved Permanently";
            _mapStatusCode["302"] = "Found";
            _mapStatusCode["400"] = "Bad Request";
            _mapStatusCode["401"] = "Unauthorized";
            _mapStatusCode["403"] = "Forbidden";
            _mapStatusCode["404"] = "Not Found";
            _mapStatusCode["405"] = "Method Not Allowed";
            _mapStatusCode["413"] = "Request Entity Too Large";
            _mapStatusCode["415"] = "Unsupported Media Type";
            _mapStatusCode["500"] = "Internal Server Error";
            _mapStatusCode["502"] = "Bad Gateway";
            _mapStatusCode["504"] = "Gateway Timeout";
            _mapStatusCode["505"] = "HTTP Version Not Supported";
        }

        void    _setCGIExtensions(void) {
            _CGIExtensions.insert(".php");
            _CGIExtensions.insert(".go");
            _CGIExtensions.insert(".py");
        }
};

#endif