#ifndef __ISERVER_HPP__
# define __ISERVER_HPP__

# include "IBuilderRoute.hpp" 
# include "EHttpStatusCode.hpp"
# include "IRoute.hpp"
# include "Logger.hpp"

class BuilderServer;

class IServer {
    private:
        virtual std::string                 _toString(void) = 0;
    public:
        virtual ~IServer(void) {}

        // Geters
        virtual std::set<std::string>
                                            GetAllowMethods(void) = 0;
        virtual std::map<HttpStatusCode::Code, std::string>
                                            GetErrorPages(void) = 0;
        virtual int                         GetBodyLimit(void) = 0;
        virtual std::string                 GetRedirectPath(std::string path) = 0;
        virtual std::string                 GetRootDirectory(void) = 0;
        virtual std::vector<std::string>    GetPageIndexes(void) = 0;
        virtual bool                        GetAutoIndex(void) = 0;
        virtual std::vector<std::string>    GetHosts(void) = 0;
        virtual std::string                 GetPort(void) = 0;
        virtual IRoute                      *GetRoute(std::string routeName) = 0;

    protected:
        // Seters
        virtual void        SetAllowMethods(std::set<std::string> methods) = 0;
        virtual void        SetErrorPage(std::set<HttpStatusCode::Code> statusCodes, std::string filePath) = 0;
        virtual void        SetBodyLimit(int size) = 0;
        virtual void        SetRedirectPath(std::pair<std::string, std::string> pair) = 0;
        virtual void        SetRootDirectory(std::string root) = 0;
        virtual void        SetPagesIndexes(std::vector<std::string> indexes) = 0;
        virtual void        SetAutoIndex(bool flag) = 0;
        virtual void        SetHosts(std::vector<std::string> hosts) = 0;
        virtual void        SetPort(std::string port) = 0;

    public:
        // Methods
        virtual void        SetRoute(std::string routeName, IRoute *route) = 0;
        virtual void        SetAddrInfo(std::string host) = 0;
        virtual void        CreateSocketAndBind(void) = 0;
        virtual int         StartListen(std::string host) = 0;
        virtual int         AcceptClientConnect(void) = 0;

        virtual IHttpResponse*          ProcessResponse(int client_fd) = 0;
        virtual void                    ProcessRequest(HttpRequest &request, int client_fd) = 0;
        virtual HttpStatusCode::Code                    ProcessRequest(HttpRequest &request, int client_fd, int* cgifd, int epoll) = 0;
        virtual std::string             FindMatchRoute(HttpRequest &res) = 0;
        virtual void                    CreateCGIResponse(int epollfd, int cgifd, int clientfd) = 0;

        virtual int                     GetListener(void) const = 0;
        virtual std::string             GetHosts(void) const = 0;
        virtual std::string             GetIP(void) const = 0;
        virtual std::string             GetIPVersion(void) const = 0;
        virtual std::string             GetListenPort(void) const = 0;
        virtual int                     newProcessCGI(HttpRequest &req, int epollFD) = 0;


        friend class BuilderServer;
        friend class IBuilderServer;
};

#endif 