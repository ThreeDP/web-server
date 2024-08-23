#ifndef __IROUTE_HPP__
# define __IROUTE_HPP__

# include "IBuilderResponse.hpp"
# include "HttpRequest.hpp"

class IRoute {
    public:
        virtual ~IRoute(void) { }

        // Seters
        virtual void        SetRouteName(std::string routeName) = 0;
        virtual void        SetBodyLimit(int size) = 0;
        virtual void        SetAllowMethods(std::set<std::string> methods) = 0;
        virtual void        SetErrorPage(std::set<HttpStatusCode::Code> statusCodes, std::string filePath) = 0;
        virtual void        SetRedirectPath(std::string redirectPath) = 0;
        virtual void        SetRootDirectory(std::string root) = 0;
        virtual void        SetRouteIndexes(std::vector<std::string> indexes) = 0;
        virtual void        SetAutoIndex(bool flag) = 0;

        // Route Process
        virtual HttpStatusCode::Code _handlerErrorResponse(
            std::ifstream *fd,
            HttpStatusCode::Code statusCode,
            IBuilderResponse &builder
        ) = 0;

        virtual HttpStatusCode::Code ProcessRequest(
            HttpRequest &request,
            IBuilderResponse &builder
        ) = 0;

        // Geters
        virtual std::string         GetRouteName(void) = 0;
        virtual int                 GetBodyLimit(void) = 0;
        virtual bool                IsAllowMethod(std::string method) = 0;
        virtual std::string         GetErrorPage(HttpStatusCode::Code code) = 0;
        virtual std::string         GetRedirectPath(void) = 0;
        virtual std::string         GetRootDirectory(void) = 0;
        virtual const std::vector<std::string>    GetRouteIndexes(void) = 0;
        virtual bool                GetAutoIndex(void) = 0;

        friend class IBuilderResponse;
};

#endif