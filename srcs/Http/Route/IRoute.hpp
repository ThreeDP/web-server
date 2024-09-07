#ifndef __IROUTE_HPP__
# define __IROUTE_HPP__

# include "IBuilderResponse.hpp"
# include "HttpRequest.hpp"

class BuilderRoute;

class IRoute {
    private:
        virtual std::string         _toString(void) = 0;

    public:
        virtual ~IRoute(void) { }
        
        // virtual IHttpResponse *ProcessRequest(
        //     HttpRequest &request
        // ) = 0;

        virtual IHttpResponse *ProcessRequest(
            HttpRequest &request,
            int* cgifd,
            int epoll
        ) = 0;

        // Geters
        virtual std::string                     GetRouteName(void) = 0;
        virtual size_t                          GetBodyLimit(void) = 0;
        virtual bool                            IsAllowMethod(std::string method) = 0;
        virtual std::string                     GetErrorPage(HttpStatusCode::Code code) = 0;
        virtual std::string                     GetRedirectPath(void) = 0;
        virtual std::string                     GetRootDirectory(void) = 0;
        virtual const std::vector<std::string>  GetRouteIndexes(void) = 0;
        virtual bool                            GetAutoIndex(void) = 0;
        virtual HttpStatusCode::Code            HandlePath(const std::string &path) = 0;

    private:
        virtual void                            SetRouteName(std::string routeName) = 0;
        virtual void                            SetBodyLimit(int size) = 0;
        virtual void                            SetAllowMethods(std::set<std::string> methods) = 0;
        virtual void                            SetErrorPage(std::set<HttpStatusCode::Code> statusCodes, std::string filePath) = 0;
        virtual void                            SetRedirectPath(std::string redirectPath) = 0;
        virtual void                            SetRootDirectory(std::string root) = 0;
        virtual void                            SetRouteIndexes(std::vector<std::string> indexes) = 0;
        virtual void                            SetAutoIndex(bool flag) = 0;

        friend class BuilderRoute;
        friend class IBuilderRoute;
};

#endif