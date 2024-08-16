#ifndef __ISERVER_HPP__
# define __ISERVER_HPP__

# include "BuilderRoute.hpp" 

class IServer {
    public:
        ~IServer(void) {}

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

        // Seters
        virtual void        SetAllowMethods(std::set<std::string> methods) = 0;
        virtual void        SetErrorPage(std::set<HttpStatusCode::Code> statusCodes, std::string filePath) = 0;
        virtual void        SetBodyLimit(int size) = 0;
        virtual void        SetRedirectPath(std::pair<std::string> pair) = 0;
        virtual void        SetRootDirectory(std::string root) = 0;
        virtual void        SetPagesIndexes(std::vector<std::string> indexes) = 0;
        virtual void        SetAutoIndex(bool flag) = 0;
        virtual void        SetHosts(std::vector<std::string> hosts) = 0;
        virtual void        SetPort(std::string port) = 0;
        virtual void        SetRoute(IRoute *route, std::string routeName) = 0;

        // Methods
        virtual void        SetAddrInfo(void) = 0;
        virtual void        CreateSocketAndBind(void) = 0;
        virtual int         StartListen(void) = 0;
        virtual int         AcceptClientConnect(void) = 0;

        virtual std::string             ProcessResponse(int client_fd) = 0;
        virtual void                    ProcessRequest(HttpRequest &request, int client_fd) = 0;
        virtual std::string             FindMatchRoute(HttpRequest &res) = 0;

        virtual int                     GetListener(void) const = 0;
        virtual std::string             GetHosts(void) const = 0;
        virtual std::string             GetIP(void) const = 0;
        virtual std::string             GetIPVersion(void) const = 0;
        virtual std::string             GetListenPort(void) const = 0;
        virtual ServerStages            GetStage(void) const = 0;
        virtual int                     GetClientFD(void) const = 0;

        friend class IBuilderServer;
};

#endif 