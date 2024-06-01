#ifndef __ROUTE_RESPONSE_HPP__
# define __ROUTE_RESPONSE_HPP__
# include <iostream>
# include <sstream>

class RouteResponse {
    private:
        std::string _body;
        int         _statusCode;
		std::string	_contenttype;
		std::map<int, std::string> _status;

    public:

        RouteResponse(std::string b, int sc) :
        _body(b), _statusCode(sc) {
            _status[200] = "OK\n";
            _status[201] = "Created\n";
            _status[202] = "Accepted\n";
            _status[204] = "No Content\n";
            _status[300] = "Multiple Choice\n";
            _status[301] = "Moved Permanently\n";
            _status[302] = "Found\n";
            _status[400] = "Bad Request\n";
            _status[401] = "Unauthorized\n";
            _status[403] = "Forbidden\n";
            _status[404] = "Not Found\n";
            _status[405] = "Method Not Allowed\n";
            _status[413] = "Request Entity Too Large\n";
            _status[415] = "Unsupported Media Type\n";
            _status[500] = "Internal Server Error\n";
            _status[502] = "Bad Gateway\n";
            _status[504] = "Gateway Timeout\n";
            _status[505] = "HTTP Version Not Supported\n";
        }

        std::string getBody(void) const {
           return this->_body; 
        }

        std::string getStatusCode(void) const {
			std::stringstream str;
			str << this->_statusCode;
            return str.str();
        }

        std::string getStatusName(void) {
            return this->_status[this->_statusCode];
        }

		std::string getContentType(void) const {
			return this->_contenttype;
		}
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