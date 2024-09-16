#ifndef __HTTP_RESPONSE_HPP__
# define __HTTP_RESPONSE_HPP__

# include "ILogger.hpp"
# include "Utils.hpp"
# include "IHttpResponse.hpp"
# include "IBuilderResponse.hpp"

class HttpResponse : public IHttpResponse {

        const std::string                   _server;
	protected:
		std::string                         _HTTPVersion;
        std::string                         _statusCode;
        std::string                         _statusMessage;
        std::map<std::string, std::string>  _headers;
        std::vector<char>                   _body;
        ILogger                             *_logger;

        // Geters
        std::string                         GetHttpVersion(void) const;
        std::pair<std::string, std::string> GetHeader(std::string key);
        std::map<std::string, std::string>  GetHeaders(void) const;
        std::string                         GetBody(void) const;
        ssize_t                             GetBodySize(void) const;
        std::string                         GetTextContent(std::string extension);
        std::string                         GetStatusMessage(void) const;
        std::string                         GetStatusCode(void) const;

	public:

        // Create Response
        std::vector<char>       CreateResponse(void);

        // Constructors
        HttpResponse(ILogger *logger);
        static void SetDefaultHTTPResponse(void) { static HttpResponse base = HttpResponse(0); }
        ~HttpResponse(void);
    
    private:
        HttpResponse(int num);
        std::string             _toString(void);
        void                    _defaultErrorPage(void);
        void                    _createBodyByDirectory(std::set<std::string> dirent, std::string path, IHandler &handler);

        // Seters
        void                    SetBody(std::vector<char> body);
        void                    SetStatusCode(HttpStatusCode::Code statusCode);
        void                    SetHeader(std::string key, std::string value);
        void                    SetBody(std::string body);

        static std::map<HttpStatusCode::Code, std::string>  _mapStatusCode;
        static std::set<std::string>                        _CGIExtensions;
        static std::map<std::string, std::string>           _mapTextContent;
        
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
            _mapStatusCode[HttpStatusCode::_CONTINUE] = "Continue";
            _mapStatusCode[HttpStatusCode::_OK] = "OK";
            _mapStatusCode[HttpStatusCode::_CREATED] = "Created";
            _mapStatusCode[HttpStatusCode::_ACCEPTED] = "Accepted";
            _mapStatusCode[HttpStatusCode::_NO_CONTENT] = "No Content";
            _mapStatusCode[HttpStatusCode::_MUILTIPLE_CHOICE] = "Multiple Choice";
            _mapStatusCode[HttpStatusCode::_MOVED_PERMANENTLY] = "Moved Permanently";
            _mapStatusCode[HttpStatusCode::_FOUND] = "Found";
            _mapStatusCode[HttpStatusCode::_TEMPORARY_REDIRECT] = "Temporary Redirect";
            _mapStatusCode[HttpStatusCode::_PERMANENT_REDIRECT] = "Permanent Redirect";
            _mapStatusCode[HttpStatusCode::_BAD_REQUEST] = "Bad Request";
            _mapStatusCode[HttpStatusCode::_UNAUTHORIZED] = "Unauthorized";
            _mapStatusCode[HttpStatusCode::_FORBIDDEN] = "Forbidden";
            _mapStatusCode[HttpStatusCode::_NOT_FOUND] = "Not Found";
            _mapStatusCode[HttpStatusCode::_METHOD_NOT_ALLOWED] = "Method Not Allowed";
            _mapStatusCode[HttpStatusCode::_LENGTH_REQUIRED] = "Length Required";
            _mapStatusCode[HttpStatusCode::_URI_TOO_LONG] = "URI Too Long";
            _mapStatusCode[HttpStatusCode::_CONTENT_TOO_LARGE] = "Content Too Large";
            _mapStatusCode[HttpStatusCode::_UNSUPPORTED_MEDIA_TYPE] = "Unsupported Media Type";
            _mapStatusCode[HttpStatusCode::_INTERNAL_SERVER_ERROR] = "Internal Server Error";
            _mapStatusCode[HttpStatusCode::_NOT_IMPLEMENTED] = "Not Implemented";
            _mapStatusCode[HttpStatusCode::_BAD_GATEWAY] = "Bad Gateway";
            _mapStatusCode[HttpStatusCode::_GATEWAY_TIMEOUT] = "Gateway Timeout";
            _mapStatusCode[HttpStatusCode::_HTTP_VERSION_NOT_SUPPORTED] = "HTTP Version Not Supported";
        }

        static void    _setCGIExtensions(void) {
            _CGIExtensions.insert(".php");
            _CGIExtensions.insert(".go");
            _CGIExtensions.insert(".py");
        }

    friend class IBuilderResponse;
};

#endif