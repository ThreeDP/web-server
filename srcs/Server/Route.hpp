#ifndef __ROUTE_HPP__
# define __ROUTE_HPP__

class Route {
    private:
        std::vector<std::string>            _allowMethods;
        std::string                         _redirectPath;
        std::string                         _directory;

    public:
        Route(std::vector<std::string> methods, std::string redirect, std::string directory);
}

#endif