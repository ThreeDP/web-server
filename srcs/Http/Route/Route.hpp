#ifndef __ROUTE_HPP__
# define __ROUTE_HPP__

# include "IRoute.hpp"
# include "IServer.hpp"

enum RouteStages {
    R_START,
    R_REQUEST
};

class BuilderRoute;

class Route : public IRoute {
    private:
        ILogger                                     *_logger;
        IHandler                                    *_handler;

        std::string                                 _route_name;
        std::set<std::string>                       _allow_methods;
        std::map<HttpStatusCode::Code, std::string> _error_page;
        int                                         _limit_client_body_size;
        std::string                                 _redirectPath;
        std::string                                 _root;
        bool                                        _autoIndex;
        std::vector<std::string>                    _indexes;
    public:
        void        SetRouteName(std::string routeName);
        void        SetBodyLimit(int size);
        void        SetAllowMethods(std::set<std::string> methods);
        void        SetErrorPage(std::set<HttpStatusCode::Code> statusCodes, std::string filePath);
        void        SetRedirectPath(std::string redirectPath);
        void        SetRootDirectory(std::string root);
        void        SetRouteIndexes(std::vector<std::string> indexes);
        void        SetAutoIndex(bool flag);
    private:
        Route(void) {}

        std::string         _toString(void);

    public:


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

        class Except: virtual public std::exception {
			protected:
			std::string error_message;
			public:
				explicit Except(const std::string& msg) : error_message(msg) {}
				virtual ~Except() throw () {}
				virtual const char* what() const throw () { return error_message.c_str();}
		};

        friend class IBuilderResponse;
};

#endif
