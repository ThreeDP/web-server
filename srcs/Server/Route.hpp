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

        RouteResponse(std::string extensionFile, DIR *directory, int _statusCode) {
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
        std::string                                 _root;
        bool                                        _autoIndex;
        std::set<std::string>                       &_index;
        
        AHttpResponse               *_fondStatusResponse(int statusCode, std::string extension);

    public:
        RouteStages                                 _stage;

        // Route Methods

        RouteResponse *_handlerErrorResponse(std::ifstream *fd, int statusCode) {
            std::string path;
            std::map<int, std::string>::iterator it = this->_error_page.find(statusCode);
            if (it != this->_error_page.end()) {
                if (!this->_handler->FileExist(it->second)) {
                    return new RouteResponse(".html", fd, statusCode);
                }
                path = this->_root + this->_error_page[statusCode];
                fd = this->_handler->OpenFile(path);
            }
            return new RouteResponse(Utils::GetFileExtension(path), fd, statusCode);
        }

        RouteResponse *ProcessRequest(HttpRequest &request) {
            std::ifstream   *fd = NULL;
            std::string     absolutePath = this->_root + request.GetPath();

            // std::cout << request << std::endl;
            std::cout << absolutePath << std::endl;
            if (!this->IsAllowMethod(request.GetMethod())) {
                return this->_handlerErrorResponse(fd, 405);
            } else if (this->_limit_client_body_size < request.GetBodySize()) {
                return this->_handlerErrorResponse(fd, 413);
            } else if (this->GetRedirectPath() != "") {
                return new RouteResponse(fd, 308, this->GetRedirectPath());
            }
            if (this->_handler->IsAllowToGetFile(absolutePath))
            {
                if (this->_handler->FileIsDirectory(absolutePath)) {
                    DIR *directory = this->_handler->OpenDirectory(absolutePath);
                    return new RouteResponse(
                        Utils::GetFileExtension(absolutePath),
                        directory,
                        200
                    );
                }
                fd = this->_handler->OpenFile(absolutePath);
                return new RouteResponse(
                    Utils::GetFileExtension(absolutePath),
                    fd,
                    200
                );
            }
            return this->_handlerErrorResponse(fd, 404);
        }

        std::set<std::string>       *CatDirectorysFiles(std::string path, std::vector<struct dirent *> &dirs);
        RouteResponse              *ProcessRoute(HttpRequest &httpReq);
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

        Route(CommonParameters *server, std::string route_name);
        Route(IServer *server, std::string route_name, IHandler *handler);

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
