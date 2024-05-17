#include "Route.hpp"

// Route::Route(std::vector<std::string> methods, std::string redirect, std::string directory) : _allowMethods(methods), _redirectPath(redirect), _directory(directory) {
//     std::cout << "Start a route!" << std::endl;
// }

void    Route::SetAllowMethods(std::vector<std::string> methods) {
    this->_allowMethods = std::vector<std::string>(methods);
}

void    Route::SetRedirectPath(std::pair<std::string, std::string> redirect) {
    this->_redirectPath[redirect.first] = redirect.second;
}

std::vector<std::string>    &Route::GetAllowMethods(void) {
    return this->_allowMethods;
}

std::map<std::string, std::string> &Route::GetRedirectPath(void) {
    return this->_redirectPath;
}