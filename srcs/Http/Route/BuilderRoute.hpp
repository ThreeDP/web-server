#ifndef __BUILDER_ROUTE_HPP__
# define __BUILDER_ROUTE_HPP__

# include "IBuilderRoute.hpp"
# include "IRoute.hpp"
# include "IServer.hpp"


class BuilderRoute : public IBuilderRoute {
    private:
        IHandler    *_handler;
        IServer     *_server;
        IRoute      *_route;

    public:
        BuilderRoute(IServer *server, IHandler *handler);
        ~BuilderRoute(void);

        IBuilderRoute &SetupRoute(std::string route_name);

        IBuilderRoute &WithBodyLimit(int size);
        IBuilderRoute &WithAllowMethods(std::set<std::string> methods);
        IBuilderRoute &WithErrorPage(std::set<HttpStatusCode::Code> statusCodes, std::string filePath);
        IBuilderRoute &WithRedirectPath(std::string redirectPath);
        IBuilderRoute &WithRootDirectory(std::string root);
        IBuilderRoute &WithPageIndexes(std::vector<std::string> indexes);
        IBuilderRoute &WithAutoIndex(bool flag);

        IRoute        *GetResult(void);
        // bool          CompareRoutes(IRoute &left, IRoute &right) = 0;
};

#endif