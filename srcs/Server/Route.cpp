#include "Route.hpp"

Route::Route(std::vector<std::string> methods, std::string redirect, std::string directory) : _allowMethods(methods), _redirectPath(redirect), _directory(directory) {
    std::cout << "Start a route!" << std::endl;
}