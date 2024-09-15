#ifndef __IBUILDER_ROUTE_HPP__
# define __IBUILDER_ROUTE_HPP__
# include "Route.hpp"
# include "EHttpStatusCode.hpp"
# include <set>
# include "IRoute.hpp"
# include "IServer.hpp"

class IBuilderRoute {
    public:
        virtual ~IBuilderRoute(void) {}

        virtual IBuilderRoute &SetupRoute(std::string route_name) = 0;

        virtual IBuilderRoute &WithBodyLimit(int size) = 0;
        virtual IBuilderRoute &WithAllowMethods(std::set<std::string> methods) = 0;
        virtual IBuilderRoute &WithErrorPage(std::set<HttpStatusCode::Code> statusCodes, std::string filePath) = 0;
        virtual IBuilderRoute &WithRedirectPath(std::string redirectPath) = 0;
        virtual IBuilderRoute &WithRootDirectory(std::string root) = 0;
        virtual IBuilderRoute &WithUploadLocation(std::string route) = 0;
        virtual IBuilderRoute &WithPageIndexes(std::vector<std::string> indexes) = 0;
        virtual IBuilderRoute &WithAutoIndex(bool flag) = 0;
        virtual IRoute        *GetResult(void) = 0;
};

#endif