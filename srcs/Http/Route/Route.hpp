#ifndef __ROUTE_HPP__
# define __ROUTE_HPP__

# include "define.hpp"
# include "Utils.hpp"
# include "HttpResponse.hpp"
# include "HttpRequest.hpp"
# include "IHandler.hpp"
# include "IRoute.hpp"
# include "IServer.hpp"

enum RouteStages {
    R_START,
    R_REQUEST
};

class BuilderRoute;

typedef HttpStatusCode::Code (Route::*Method)(HttpRequest &request);

class Route : public IRoute {
    private:
        std::string                                 _route_name;
        std::set<std::string>                       _allow_methods;
        std::map<HttpStatusCode::Code, std::string> _error_page;
        int                                         _limit_client_body_size;
        std::string                                 _redirectPath;
        std::string                                 _root;
        bool                                        _autoIndex;
        std::vector<std::string>                    _indexes;
        ILogger                                     *_logger;
        IHandler                                    *_handler;
        IBuilderResponse                            *_builder;
        std::map<std::string, Method>               _methods;

        void        SetRouteName(std::string routeName);
        void        SetBodyLimit(int size);
        void        SetAllowMethods(std::set<std::string> methods);
        void        SetErrorPage(std::set<HttpStatusCode::Code> statusCodes, std::string filePath);
        void        SetRedirectPath(std::string redirectPath);
        void        SetRootDirectory(std::string root);
        void        SetRouteIndexes(std::vector<std::string> indexes);
        void        SetAutoIndex(bool flag);

        std::string _toString(void);

    public:

        HttpStatusCode::Code _errorHandlerWithFile(HttpStatusCode::Code statusCode) {
            std::string path;
            std::ifstream *fd = NULL;

            bool hasErrorPage = (path = this->GetErrorPage(statusCode)) != "";
            std::string abspath = Utils::SanitizePath(this->_root, this->_route_name);
            path = Utils::SanitizePath(abspath, path);
            if (hasErrorPage && this->_handler->FileExist(path)) {
                if (this->_handler->IsAllowToGetFile(path)) {
                    fd = this->_handler->OpenFile(path);
                    _builder
                        ->SetupResponse()
                        .WithStatusCode(statusCode)
                        .WithLastModified(path)
                        .WithContentType(Utils::GetFileExtension(path))
                        .WithFileDescriptor(fd);
                    std::cout << _logger->Log(&Logger::LogInformation, "Response By Setup Erro Pages: ", statusCode) << std::endl;
                    return (statusCode);
                }
            }
            return HttpStatusCode::_DO_NOTHING;
        }

        HttpStatusCode::Code _errorHandlerDefault(HttpStatusCode::Code statusCode) {
            builder
                ->SetupResponse()
                .WithStatusCode(statusCode)
                .WithContentType(".html")
                .WithDefaultPage();
            
            std::cout << _logger->Log(&Logger::LogInformation, "Response By Default Error: ", statusCode) << std::endl;
            return (statusCode);
        }

        HttpStatusCode::Code _checkAllowMethod(std::string method) {
            if (!this->IsAllowMethod(request.GetMethod())) {
                HttpStatusCode::Code status = HttpStatusCode::_METHOD_NOT_ALLOWED;
                HttpStatusCode::Code result = HttpStatusCode::_DO_NOTHING;

                if ((result = this->_errorHandlerWithFile(status))) { return result; }
                return this->_errorHandlerDefault(status);
            }
            return HttpStatusCode::_DO_NOTHING;
        }

        HttpStatusCode::Code _checkBodyLimit(size_t limit) {
            if (this->GetBodyLimit() < request.GetBodySize()) {
                HttpStatusCode::Code status = HttpStatusCode::_CONTENT_TOO_LARGE;
                HttpStatusCode::Code result = HttpStatusCode::_DO_NOTHING;
                
                if ((result = this->_errorHandlerWithFile(status))) { return result; }
                return this->_errorHandlerDefault(status);
            }
            return HttpStatusCode::_DO_NOTHING;
        }

        HttpStatusCode::Code _notFound(void) {
            HttpStatusCode::Code status = HttpStatusCode::_NOT_FOUND;
            HttpStatusCode::Code result = HttpStatusCode::_DO_NOTHING;
            
            if ((result = this->_errorHandlerWithFile(status))) { return result; }
            return this->_errorHandlerDefault(status);
        }

        HttpStatusCode::Code _checkRedirectPath(std::string path) {
            HttpStatusCode::Code result = HttpStatusCode::_DO_NOTHING;

            if (path != "") {
                result = HttpStatusCode::_PERMANENT_REDIRECT;

                builder
                    .SetupResponse()
                    .WithStatusCode(result)
                    .WithLocation("/" + path)
                    .WithDefaultPage();
                std::cout << _logger->Log(&Logger::LogInformation, "Permanent Redirect: ", HttpStatusCode::_PERMANENT_REDIRECT) << std::endl;
                return (result);
            }
            return result;
        }

        HttpStatusCode::Code Post(HttpRequest &request) {
            if ((result = this->_checkBodyLimit(request.GetBodyLimit()))) { return result; }
            if (this->_handler->PathExist(absolutePath)) {
                
            }
            return this->_notFound();
        }

        HttpStatusCode::Code Get(HttpRequest &request) {
            if ( request.GetBodySize() > 0 ) { return HttpStatusCode::_BAD_REQUEST }
            if (this->_handler->PathExist(absolutePath)) {
                bool isDirectory = this->_handler->FileIsDirectory(absolutePath);
                bool allow = this->_handler->IsAllowToGetFile(absolutePath);
                if (allow && isDirectory) {
                    if (( result = this->_checkExistIndex(request.GetPath(), absolutePath, isDirectory, allow) )) { return result; }
                }
            }
            return this->_notFound();
        }

        HttpStatusCode::Code Delete(HttpRequest &request) {
            if ((result = this->_checkBodyLimit(request.GetBodyLimit()))) { return result; }
            if (this->_handler->PathExist(absolutePath)) {
                
            }
            return this->_notFound();
        }

        HttpStatusCode::Code _checkExistIndex(std::string reqPath, std::string absPath, bool isDir, bool allowGet) {
            std::vector<std::string>::iterator it = this->_indexes.begin();
            for ( ; it != this->_indexes.end(); ++it)
            {
                std::string pathAutoindex = absolutePath;
                pathAutoindex = Utils::SanitizePath(pathAutoindex, *it);
                std::cout << pathAutoindex << std::endl;
                if (this->_handler->PathExist(pathAutoindex)
                && this->_handler->IsAllowToGetFile(pathAutoindex)
                && !this->_handler->FileIsDirectory(pathAutoindex))
                {
                    builder
                        .SetupResponse()
                        .WithStatusCode(HttpStatusCode::_FOUND)
                        .WithLocation(Utils::SanitizePath("http://localhost:8081",
                            Utils::SanitizePath(request.GetPath(), *it)))
                        .WithDefaultPage();
                    std::cout << _logger->Log(&Logger::LogInformation, "Redirect by index: ", HttpStatusCode::_FOUND) << std::endl;
                    return (HttpStatusCode::_FOUND);
                }
            }
            return HttpStatusCode::_DO_NOTHING;
        }


        Route(void) {}

        Route(ILogger *logger, IServer *server, IHandler *handler, std::string route_name);
        ~Route(void);

        // Route Process
        HttpStatusCode::Code _handlerErrorResponse(
            std::ifstream *fd,
            HttpStatusCode::Code statusCode,
            IBuilderResponse &builder
        );
        
        HttpStatusCode::Code ProcessRequest(
            HttpRequest &request,
            IBuilderResponse &builder
        );
        
        // Geters
        std::string                         GetRouteName(void);
        int                                 GetBodyLimit(void);
        bool                                IsAllowMethod(std::string method);
        std::string                         GetErrorPage(HttpStatusCode::Code code);
        std::string                         GetRedirectPath(void);
        std::string                         GetRootDirectory(void);
        const std::vector<std::string>      GetRouteIndexes(void);
        bool                                GetAutoIndex(void);

        // Base methods

        class Except: virtual public std::exception {
			protected:
			std::string error_message;
			public:
				explicit Except(const std::string& msg) : error_message(msg) {}
				virtual ~Except() throw () {}
				virtual const char* what() const throw () { return error_message.c_str();}
		};

        friend class IBuilderResponse;
        friend class BuilderResponse;
};

#endif
