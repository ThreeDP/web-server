#ifndef __ROUTE_RESPONSE_HPP__
# define __ROUTE_RESPONSE_HPP__

# include "IHandler.hpp"

class RouteResponse {
    
    public:
        std::ifstream   *FD;
        int             StatusCode;
        std::string     RedirectPath;
        DIR             *Directory;
        std::string     FileExtension;
        
        RouteResponse(std::ifstream *_fd, int _statusCode) {
            this->FD = _fd;
            this->StatusCode = _statusCode;
            this->Directory = NULL;
            this->RedirectPath = "";
            this->FileExtension = "text";
        }

        RouteResponse(std::string extensionFile, int _statusCode, DIR *directory) {
            this->FD = NULL;
            this->StatusCode = _statusCode;
            this->Directory = directory;
            this->RedirectPath = "";
            if (extensionFile == "")
                this->FileExtension = "text";
            else
                this->FileExtension = extensionFile;
        }

        RouteResponse(std::string extensionFile, std::ifstream *_fd, int _statusCode) {
            this->FD = _fd;
            this->StatusCode = _statusCode;
            this->Directory = NULL;
            this->RedirectPath = "";
            if (extensionFile == "")
                this->FileExtension = "text";
            else
                this->FileExtension = extensionFile;
        }

        RouteResponse(std::ifstream *_fd, int _statusCode, std::string redirectPath) {
            this->FD = _fd;
            this->StatusCode = _statusCode;
            this->RedirectPath = redirectPath;
            this->Directory = NULL;
            this->FileExtension = "text";
        }

        bool operator==(const RouteResponse &other) const {
            return
                StatusCode == other.StatusCode &&
                RedirectPath == other.RedirectPath &&
                FileExtension == other.FileExtension;
        }
};

std::ostream &operator<<(std::ostream &os, RouteResponse const &route);


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

