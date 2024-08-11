#ifndef __HTTP_RESPONSE_HPP__
# define __HTTP_RESPONSE_HPP__

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
        void                    _createBodyByDirectory(std::set<struct dirent*> *dirent, std::string path, IHandler &handler);
        void                    _createBodyByFileDescriptor(std::ifstream *fd);
        std::string             GetHttpVersion(void) const;
        std::string             GetStatusCode(void) const;
        std::string             GetStatusMessage(void) const;
        std::pair<std::string, std::string> GetHeader(std::string key);
        std::map<std::string, std::string>  GetHeaders(void) const;
        std::string             GetBody(void) const;

	public:

        // Create Response
        std::string             CreateResponse(void);
        std::string             ToString(void);

        // Geters
        std::string             GetStatusName(HttpStatusCode::Code statusCode);
        std::string             GetTextContent(std::string extension);

        // Seters
        static void SetDefaultHTTPResponse(void) {
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

// Response::mimetypes_map Response::mimetypes = Response::init_mimetypes();
// Response::mimetypes_map Response::init_mimetypes(void) {
//   mimetypes_map _map;

//   _map["text"] = "Content-Type: text/plain\n";
//   _map[".txt"] = "Content-Type: text/plain; charset=utf-8\n";
//   _map[".html"] = "Content-Type: text/html; charset=utf-8\n";
//   _map[".css"] = "Content-Type: text/css; charset=utf-8\n";
//   _map[".jpg"] = "Content-type: image/jpg\n";
//   _map[".jpeg"] = "Content-type: image/jpeg\n";
//   _map[".png"] = "Content-type: image/png\n";
//   _map[".mp4"] = "Content-type: video/mp4\n";
//   _map[".ico"] = "Content-type: image/vnd.microsoft.icon\n";
//   _map[".php"] = "Content-Type: text/plain; charset=utf-8\n";
//   _map[".js"] = "Content-Type: application/javascript\n";
//   _map[".gif"] = "Content-Type: image/gif\n";
//   return _map;
// }

// Response::meth_map Response::method_map = Response::init_map();
// Response::meth_map Response::init_map(void) {
//   meth_map _map;

//   _map["GET"] = &Response::_get;
//   _map["POST"] = &Response::_post;
//   _map["DELETE"] = &Response::_delete;
//   _map["PUT"] = &Response::_put;
//   _map["HEAD"] = &Response::_head;
//   _map["CONNECT"] = &Response::_connect;
//   _map["OPTIONS"] = &Response::_options;
//   _map["TRACE"] = &Response::_trace;
//   _map["PATCH"] = &Response::_patch;
//   return _map;
// }

#endif