#ifndef __ROUTE_RESPONSE_HPP__
# define __ROUTE_RESPONSE_HPP__

# include <iostream>
# include <sstream>
# include <map>

class RouteResponse {
    private:
        std::string _body;
        int         _statusCode;
		std::string	_contenttype;
		std::map<int, std::string> _status;

    public:
        std::string defaultErrorPage(int statusCode);

        // Geters

        std::string getBody(void) const;
        std::string getStatusCode(void) const;
        std::string getStatusName(void);
		std::string getContentType(void) const;

        // Base Methods

        RouteResponse(std::string b, int sc);
        RouteResponse(int sc);
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

