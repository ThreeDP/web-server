#ifndef __ROUTE_HPP__
# define __ROUTE_HPP__

#include <iostream>
#include <vector>
#include <map>

class Route {
    private:
        std::vector<std::string>            _allowMethods;
        std::map<std::string, std::string>  _redirectPath;
        std::string                         _directory;

    public:
        std::map<std::string, std::string> &GetRedirectPath(void);
        std::vector<std::string>    &GetAllowMethods(void);
        void    SetAllowMethods(std::vector<std::string> methods);
        void    SetRedirectPath(std::pair<std::string, std::string> redirect);

        Route(){}
        //Route(std::vector<std::string> methods, std::string redirect, std::string directory);
};

#endif