#include "Route.hpp"

// Route::Route(std::vector<std::string> methods, std::string redirect, std::string directory) : _allowMethods(methods), _redirectPath(redirect), _directory(directory) {
//     std::cout << "Start a route!" << std::endl;
// }

// Route Methods
std::vector<std::string>     *Route::CatDirectorysFiles(std::string path) {
    DIR                         *dir = NULL;
    std::vector<std::string>    *directorys = new std::vector<std::string>();

    dir = opendir(path.c_str());
    if (dir != NULL) {
        struct dirent* entry;

        while ((entry = readdir(dir)) != NULL) {
            std::string d_name = entry->d_name;
            directorys->push_back(d_name);
        }
        closedir(dir);
    }
    return directorys;
}

std::string    Route::ProcessRoute(std::string path) {
    std::string body;
    std::string newP = this->_directory;
    newP += path;

    return this->DetermineOutputFile(newP);
}

std::string Route::ReturnFileRequest(std::string path) {
    std::string body;

    std::ifstream file(path.c_str());
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            body += line;
        }
        file.close();
    }
    return body;
}

mode_t Route::CatFileMode(std::string path) {
    struct stat sb;

    memset(&sb, 0, sizeof(struct stat));
    if (stat(path.c_str(), &sb) == -1) {
        std::cerr << "Error on check file type";
    }
    return sb.st_mode;
}

bool Route::FindFilePattern(std::string &path, std::vector<std::string> *dirs) {
    std::vector<std::string>::iterator it = dirs->begin();
    for (; it != dirs->end(); ++it) {
        if (*it == "index.html") {
            path += *it;
            return true;
        }
    }
    return false;
}

std::string Route::DetermineOutputFile(std::string path) {
    std::stringstream body;
    bool exitCheck = false;
    while (1) {
        switch (CatFileMode(path) & S_IFMT) {
        case S_IFDIR:
            std::vector<std::string> *dirs;
            dirs = this->CatDirectorysFiles(path);
            if (!this->FindFilePattern(path, dirs)) {
                // int status = this->checkFilePermission(path);
                for (int i = 0; i < dirs->size(); i++) {
                    body << (*dirs)[i] << "\r\n";
                }
                delete dirs;
                exitCheck = true;
            }
            break;
        case S_IFREG:
            // int status = this->checkFilePermission(path);
            body << this->ReturnFileRequest(path);
            exitCheck = true;
            break;
        default:
            exitCheck = true;
            std::cout << "default" << std::endl;
            break;
        }
        if (exitCheck)
            break;
    }
    return body.str();
}


// Geters
std::set<std::string>    &Route::GetAllowMethods(void) {
    return this->_allow_methods;
}

std::string Route::GetRedirectPath(void) {
    return this->_redirectPath;
}

// Seters
void    Route::SetAllowMethods(std::set<std::string> &methods) {
    this->_allow_methods = methods;
}

void    Route::SetRedirectPath(std::string redirect) {
    this->_redirectPath = redirect;
}

// Base Methods
Route::Route(CommonParameters *server, std::string server_name)  : 
    _allow_methods(server->GetDefaultAllowMethods()),
    _error_page(server->GetDefaultErrorPage()),
    _limit_client_body_size(2048),
    _directory(server->GetRoot()),
    _autoIndex(server->GetAutoIndex()),
    _index(server->GetIndex())
{
    std::map<std::string, std::string>::iterator it = server->GetReWrites().find(server_name);
    if (server->GetReWrites().find(server_name) != server->GetReWrites().end())
        this->_redirectPath = server->GetReWrites()[server_name];
    std::cout << "Create Route" << std::endl;
}