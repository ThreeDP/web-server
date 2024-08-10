#ifndef __ROUTE_HPP__
# define __ROUTE_HPP__

# include "define.hpp"
# include "Utils.hpp"
# include "CommonParameters.hpp"
# include "HttpResponse.hpp"
# include "HttpRequest.hpp"
# include "IHandler.hpp"
# include "RouteResponse.hpp"
# include "BuilderResponse.hpp"


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
        
        IHttpResponse               *_fondStatusResponse(int statusCode, std::string extension);

    public:
        RouteStages                                 _stage;

        // Route Methods

        HttpStatusCode::Code _handlerErrorResponse(
            std::ifstream *fd,
            HttpStatusCode::Code statusCode,
            IBuilderResponse &builder
        ) {
            std::string path;

            bool hasErrorPage = (path = this->GetErrorPage(statusCode)) != "";
            path = Utils::SanitizePath(this->_root, path);
            if (hasErrorPage && this->_handler->FileExist(path)) {
                if (this->_handler->IsAllowToGetFile(path)) {
                    fd = this->_handler->OpenFile(path);
                    builder
                        .WithStatusCode(statusCode)
                        .WithFileDescriptor(fd)
                        .WithContentType(Utils::GetFileExtension(path));
                    return (statusCode);
                        // builder = new BuilderResponse(this->_handler, statusCode);
                        // builder->SetFileDescriptor(fd)
                        //     .SetFileExtension(Utils::GetFileExtension(path)),
                        //     builder);

                    // return new RouteResponse(Utils::GetFileExtension(path), fd, statusCode);
                }
            }
            builder
                .WithStatusCode(statusCode)
                .WithContentType(".html");
            return (statusCode);
            // builder = new BuilderResponse(this->_handler, statusCode);
            // return (
            //     builder->SetFileExtension(".html"), builder
            // );
            // return new RouteResponse(".html", fd, statusCode);
        }

        HttpStatusCode::Code ProcessRequest(HttpRequest &request, IBuilderResponse &builder) {
            std::ifstream   *fd = NULL;
            std::string     absolutePath;
            
            absolutePath = Utils::SanitizePath(this->_root, request.GetPath());
            if (!this->IsAllowMethod(request.GetMethod())) {
                return this->_handlerErrorResponse(
                    fd,
                    HttpStatusCode::_METHOD_NOT_ALLOWED,
                    builder
                );
            } else if (this->_limit_client_body_size < request.GetBodySize()) {
                return this->_handlerErrorResponse(
                    fd,
                    HttpStatusCode::_CONTENT_TOO_LARGE,
                    builder
                );
            } else if (this->GetRedirectPath() != "") {
                builder
                    .WithStatusCode(HttpStatusCode::_PERMANENT_REDIRECT)
                    .WithLocation(this->GetRedirectPath());
                return (HttpStatusCode::_PERMANENT_REDIRECT);
                // builder = (new BuilderResponse(this->_handler, 308));
                // return (builder->SetRedirectPath(this->GetRedirectPath()), builder);
                // return new RouteResponse(fd, 308, this->GetRedirectPath());
            }
            if (this->_handler->PathExist(absolutePath))
            {
                bool isDirectory = this->_handler->FileIsDirectory(absolutePath);
                bool allow = this->_handler->IsAllowToGetFile(absolutePath);
                if (allow && isDirectory)
                {
                    std::string pathAutoindex = absolutePath;
                    for (std::set<std::string>::iterator it = this->_index.begin(); it != this->_index.end(); ++it)
                    {
                        pathAutoindex = Utils::SanitizePath(pathAutoindex, *it);
                        if (this->_handler->PathExist(pathAutoindex)
                        && this->_handler->IsAllowToGetFile(pathAutoindex)
                        && !this->_handler->FileIsDirectory(pathAutoindex))
                        {
                            // absolutePath = pathAutoindex;
                            // fd = this->_handler->OpenFile(absolutePath);
                            builder
                                .WithStatusCode(HttpStatusCode::_FOUND)
                                .WithLocation(Utils::SanitizePath(request.GetPath(), *it));
                            return (HttpStatusCode::_FOUND);
                            // builder = (new BuilderResponse(this->_handler, 302));
                            // return (builder->SetRedirectPath(Utils::SanitizePath(request.GetPath(), *it))
                            //     .SetFileExtension(".html")
                            //     .SetFileDescriptor(fd), builder);
                            // std::string test = ;
                            // return new RouteResponse(
                            //     fd,
                            //     302,
                            //     test
                            // );
                        }
                    }
                    if (this->_autoIndex) {
                        DIR *dir = this->_handler->OpenDirectory(absolutePath);
                        builder
                            .WithStatusCode(HttpStatusCode::_OK)
                            .WithDirectoryFile(dir)
                            .WithContentType(".html");
                        return (HttpStatusCode::_OK);
                        // builder = (new BuilderResponse(this->_handler, 200));
                        // return (builder->SetDirectory(dir)
                        //     .SetFileExtension(".html"), builder);
                    }
                } else if (allow) {
                    fd = this->_handler->OpenFile(absolutePath);
                    builder
                        .WithStatusCode(HttpStatusCode::_OK)
                        .WithContentType(Utils::GetFileExtension(absolutePath))
                        .WithFileDescriptor(fd);
                    return (HttpStatusCode::_OK);
                    // builder = (new BuilderResponse(this->_handler, 200));
                    // return (
                    //     builder->SetFileDescriptor(fd)
                    //         .SetFileExtension(Utils::GetFileExtension(absolutePath)),
                    //     builder);
                    // return new RouteResponse(
                    //     Utils::GetFileExtension(absolutePath),
                    //     fd,
                    //     200
                    // );
                } else if (!allow) {
                    return this->_handlerErrorResponse(fd, HttpStatusCode::_FORBIDDEN, builder);
                }
            }
            return this->_handlerErrorResponse(fd, HttpStatusCode::_NOT_FOUND, builder);
        }

        std::set<std::string>       *CatDirectorysFiles(std::string path, std::vector<struct dirent *> &dirs);
        RouteResponse              *ProcessRoute(HttpRequest &httpReq);
        std::string                 ReturnFileRequest(std::string path);
        mode_t                      CatFileMode(std::string &path, int &statusCode);
        bool                        FindFilePattern(std::string &path, std::set<std::string> *dirs);
        IHttpResponse               *DetermineOutputFile(HttpRequest &httpReq);
        std::string                 GenerateAutoindex(std::vector<struct dirent *> dirs, std::string path);
        IHttpResponse               *checkFilePermission(HttpRequest &httpReq, int &statusCode);
        
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
