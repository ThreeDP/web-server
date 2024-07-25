#ifndef __ROUTE_HPP__
# define __ROUTE_HPP__

# include "define.hpp"
# include "Utils.hpp"
# include "CommonParameters.hpp"
# include "AHttpResponse.hpp"
# include "HttpRequest.hpp"
# include "IHandler.hpp"
# include "Response200OK.hpp"
//# include "HttpResponse.hpp"

class RouteResponse {
    
    public:
        int fd;
        int statusCode;
        bool isDirectory;
        
        RouteResponse(int _fd, int _statusCode, bool _isDirectory) {
            this->fd = _fd;
            this->statusCode = _statusCode;
            this->isDirectory = _isDirectory;
        }

        bool operator==(const RouteResponse &other) const {
            return fd == other.fd &&
                statusCode == other.statusCode &&
                isDirectory == other.isDirectory;
        }
};

std::ostream &operator<<(std::ostream &os, RouteResponse const &route);

enum RouteStages {
    R_START,
    R_REQUEST
};

class Route {
    private:
        IHandler                                    *_handler;
        std::string                                 _route_name;
        std::set<std::string>                       *_allow_methods;
        std::map<int, std::string>                  _error_page;
        int                                         _limit_client_body_size;
        std::string                                 _redirectPath;
        std::string                                 _directory;
        bool                                        _autoIndex;
        std::set<std::string>                       &_index;
        
        AHttpResponse               *_fondStatusResponse(int statusCode, std::string extension);

    public:
        RouteStages                                 _stage;

        // Route Methods

        RouteResponse *_handlerErrorResponse(int fd, int statusCode) {
            std::map<int, std::string>::iterator it = this->_error_page.find(statusCode);
            if (it != this->_error_page.end()) {
                fd = this->_handler->OpenFile(this->_error_page[statusCode]);
            }
            return new RouteResponse(fd, statusCode, false);
        }

        RouteResponse *ProcessRequest(HttpRequest &request) {
            int fd = -1;
            int statusCode = 200;
        
            if (!this->IsAllowMethod(request.GetMethod())) {
                statusCode = 405;
                return this->_handlerErrorResponse(fd, statusCode);
            } else if (this->_limit_client_body_size < request.GetBodySize()) {
                statusCode = 413;
                return this->_handlerErrorResponse(fd, statusCode);
            }
            fd = this->_handler->OpenFile(request.GetPath());
            return new RouteResponse(fd, statusCode, false);
        }

        std::set<std::string>       *CatDirectorysFiles(std::string path, std::vector<struct dirent *> &dirs);
        AHttpResponse               *ProcessRoute(HttpRequest &httpReq);
        std::string                 ReturnFileRequest(std::string path);
        mode_t                      CatFileMode(std::string &path, int &statusCode);
        bool                        FindFilePattern(std::string &path, std::set<std::string> *dirs);
        AHttpResponse               *DetermineOutputFile(HttpRequest &httpReq);
        std::string                 GenerateAutoindex(std::vector<struct dirent *> dirs, std::string path);
        AHttpResponse               *checkFilePermission(HttpRequest &httpReq, int &statusCode);
        
        // Geters
        std::string                 GetRedirectPath(void);
        std::string                 GetErrorPage(int statusCode);
        bool                        IsAllowMethod(std::string method);
        int                         GetLimitClientBodySize(void) const;
        std::string                 GetRoot(void) const;
        std::set<std::string>       GetFilesForIndex(void) const;

        std::set<std::string>       *GetAllowMethods(void);
        std::map<int, std::string>  GetErrorPage(void);
        void                        pathReset(std::string &path);
        std::string                 GetRouteName(void) const;

        // Seters

        void                        SetAllowMethods(std::set<std::string> *methods);
        void                        SetRedirectPath(std::string redirect);
        void                        SetErrorPageRoute(std::pair<int, std::string> error_page);

        // Base methods

        Route(CommonParameters *server, std::string server_name);
        Route(IServer *server, std::string server_name, IHandler *handler);

        class Except: virtual public std::exception {
			protected:
			std::string error_message;
			public:
				explicit Except(const std::string& msg) : error_message(msg) {}
				virtual ~Except() throw () {}
				virtual const char* what() const throw () { return error_message.c_str();}
		};
};

std::ostream &operator<<(std::ostream &os, Route const &route);

#endif
