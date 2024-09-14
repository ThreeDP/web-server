#ifndef __ROUTE_HPP__
# define __ROUTE_HPP__

# include "define.hpp"
# include "Utils.hpp"
# include "HttpResponse.hpp"
# include "HttpRequest.hpp"
# include "IHandler.hpp"
# include "BuilderResponse.hpp"
# include "IRoute.hpp"
# include "IServer.hpp"
#include <sys/wait.h>

enum RouteStages {
    R_START,
    R_REQUEST
};

class BuilderRoute;

class Route : public IRoute {
    private:
        typedef HttpStatusCode::Code (Route::*Method)(HttpRequest &request, std::string);
        std::string                                 _route_name;
        std::set<std::string>                       _allow_methods;
        std::map<HttpStatusCode::Code, std::string> _error_page;
        size_t                                      _limit_client_body_size;
        std::string                                 _redirectPath;
        std::string                                 _root;
        bool                                        _autoIndex;
        std::string                                 _upload_on;
        std::vector<std::string>                    _indexes;
        ILogger                                     *_logger;
        IHandler                                    *_handler;
        IBuilderResponse                            *_builder;
        // std::map<std::string, Method>               _httpMethods;

        /**!
         * 
         * Seters
         * 
         */

        void                            SetRouteName(std::string routeName);
        void                            SetBodyLimit(int size);
        void                            SetAllowMethods(std::set<std::string> methods);
        void                            SetErrorPage(std::set<HttpStatusCode::Code> statusCodes, std::string filePath);
        void                            SetRedirectPath(std::string redirectPath);
        void                            SetRootDirectory(std::string root);
        void                            SetRouteIndexes(std::vector<std::string> indexes);
        void                            SetAutoIndex(bool flag);
        void                            SetUploadOn(std::string route);

        std::string _toString(void);

    public:

        /**!
         * 
         * Init Method:
         * Route processes a request
         * 
         */
        
        // IHttpResponse *ProcessRequest(
        //     HttpRequest &request
        // );
        IHttpResponse *ProcessRequest(
            HttpRequest &request,
            int* cgifd,
            int epoll
        );
    
        Route(void) {}
        Route(ILogger *logger, IServer *server, IHandler *handler, std::string route_name);
        ~Route(void);

        /**!
         * 
         * Geters
         * 
         */

        std::string                     GetRouteName(void);
        size_t                          GetBodyLimit(void);
        bool                            IsAllowMethod(std::string method);
        std::string                     GetErrorPage(HttpStatusCode::Code code);
        std::string                     GetRedirectPath(void);
        std::string                     GetRootDirectory(void);
        const std::vector<std::string>  GetRouteIndexes(void);
        bool                            GetAutoIndex(void);
        std::string                     GetUploadOn(void);

    private:

        /**!
         * 
         * General Assets
         * Use in all http methods GET, POST and DELETE.
         * 
         */

        HttpStatusCode::Code _errorHandler(HttpStatusCode::Code code);

        /// @brief Check if method of request
        /// is not allow in the route.
        /// @param method 
        /// @return  METHOD NOT ALLOWED
        HttpStatusCode::Code _checkAllowMethod(std::string method);

        /// @brief Check the redirect path
        /// and inject a builder response
        /// if they has a not empty path.
        /// @param path 
        /// @return PERMANENT REDIRECT
        HttpStatusCode::Code _checkRedirectPath(std::string path);

        /// @brief Check if body of request
        /// is bigger them route limit.
        /// @param limit 
        /// @return CONTENT TOO LARGE
        HttpStatusCode::Code _checkBodyLimit(size_t limit);

        HttpStatusCode::Code _checkDirectory(std::string absPath, HttpRequest &request);

        /// @brief Check if the Directory listining
        /// is allowed.
        /// @param absolutePath 
        /// @return OK
        HttpStatusCode::Code _checkAutoIndex(std::string absolutePath);

        /// @brief Check if exist a file for the error status code
        /// and return they if exist
        /// @param statusCode 
        /// @return statusCode
        HttpStatusCode::Code _errorHandlerWithFile(HttpStatusCode::Code statusCode);

        /// @brief Return a error default page.
        /// @param statusCode 
        /// @return statusCode
        HttpStatusCode::Code _errorHandlerDefault(HttpStatusCode::Code statusCode);

        /// @brief Check if a file can be
        /// executed or read
        /// @param absolutePath 
        /// @return OK
        HttpStatusCode::Code _checkActionInFile(std::string absolutePath);
        HttpStatusCode::Code _notFound(void);


        /**!
         * 
         * POST Assets
         * Use in Post for produces they rules.
         * 
         */

        // HttpStatusCode::Code Post(HttpRequest &request, std::string absPath);
        HttpStatusCode::Code Post(HttpRequest &request, std::string absPath, int* cgifd, int epoll);

        /**!
         * 
         * DELETE Assets
         * Use in Post for produces they rules.
         * 
         */

        // HttpStatusCode::Code Delete(HttpRequest &request, std::string absPath);
        HttpStatusCode::Code Delete(HttpRequest &request, std::string absPath, int epoll);


        /**!
         * 
         * GET Assets
         * Use in Post for produces they rules.
         * 
         */
        HttpStatusCode::Code        cgiAction(HttpRequest &req, std::string absPath, int* cgifd);

        // HttpStatusCode::Code Get(HttpRequest &request, std::string absPath);
        HttpStatusCode::Code Get(HttpRequest &request, std::string absPath, int* cgifd, int epoll);
    
    private:
        /// @brief Checks if there is any file
        /// in indexes valid for the route.
        /// @param reqPath 
        /// @param absPath 
        /// @return OK
        HttpStatusCode::Code _checkExistIndex(std::string reqPath, std::string absPath);

        friend class IBuilderResponse;
        friend class BuilderResponse;
};

#endif
