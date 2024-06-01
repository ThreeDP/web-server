#include "Route.hpp"

// Route::Route(std::vector<std::string> methods, std::string redirect, std::string directory) : _allowMethods(methods), _redirectPath(redirect), _directory(directory) {
//     std::cout << "Start a route!" << std::endl;
// }

// Route Methods
std::set<std::string>     *Route::CatDirectorysFiles(std::string path, std::vector<struct dirent *> &dirs) {
    DIR                         *dir = NULL;
    std::set<std::string>       *dirNames = new std::set<std::string>();

    dir = opendir(path.c_str());
    if (dir != NULL) {
        struct dirent* entry;

        while ((entry = readdir(dir)) != NULL) {
            // if (this->_autoIndex)
                dirs.push_back(entry);
            std::string d_name = entry->d_name;
            dirNames->insert(d_name);
        }
        closedir(dir);
    }
    return dirNames;
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

bool Route::FindFilePattern(std::string &path, std::set<std::string> *dirs) {
    std::set<std::string>::iterator it = this->_index.begin();
    for ( ; it != this->_index.end(); ++it) {
        std::set<std::string>::iterator i = dirs->find(*it);
        if (i != dirs->end()) {
            path += *i;
            delete dirs;
            return true;
        }
    }
    return false;
}

std::string Route::GenerateAutoindex(std::vector<struct dirent *> dirs, std::string path) {
    std::stringstream   body;

    std::string actualDir = getActualDir(path);
    body << "<html data-lt-installed=\"true\">" << "\r\n";
    body << "<head>\r\n";
    body << "\t<title>Index of " << actualDir << "</title>\r\n";
    body << "</head>\r\n";
    body << "<body>\r\n";
    body << "\t<h1>index of " << actualDir << "</h1>\r\n";
    body << "\t<hr>\r\n";
    body << "\t<pre>\r\n";
    std::vector<struct dirent *>::iterator it = dirs.begin();
    for (; it != dirs.end(); ++it) {
        std::string filePath = path + std::string((*it)->d_name);
        if ((*it)->d_type == DT_DIR && std::string((*it)->d_name) != ".." && std::string((*it)->d_name) != ".")
            body << "\t\t<a href=\"" << (*it)->d_name << "/\">" << (*it)->d_name << "/</a>\r\n";
        else
            body << "\t\t<a href=\"" << (*it)->d_name << "\">" << (*it)->d_name << "</a>\r\n";
        body << "\t\t" << Route::getLastModifiedOfFile(filePath) << " " << Route::getFileSize(filePath) << "\r\n";
    }
    body << "\t</prev>\r\n";
    body << "\t<hr>\r\n";
    body << "</body>\r\n";
    body << "</html>";
    return body.str();
}

std::string Route::DetermineOutputFile(std::string path) {
    std::stringstream body;
    bool exitCheck = false;
    while (1) {
        std::set<std::string> *dirNames = NULL;
        std::vector<struct dirent *> dirs;
        switch (CatFileMode(path) & S_IFMT) {
        case S_IFDIR:
            dirNames = this->CatDirectorysFiles(path, dirs);
            if (dirNames != NULL && !this->FindFilePattern(path, dirNames)) {
                // int status = this->checkFilePermission(path);
                body << GenerateAutoindex(dirs, path);
                delete dirNames;
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

// Statics Functions
std::string Route::getActualDir(std::string path) {
    int size = path.size();
    if (size && --size && path[size] == '/')
        --size;
    else
        path += "/";
    while (path[size] != '/' && size >= 0) {
        --size;
    }
    if (size < 0)
        size = 0;
    return path.substr(size, path.size());
}

std::string	Route::getLastModifiedOfFile(const std::string &filename) {
	struct stat stat_buff;
	stat(filename.c_str(), &stat_buff);
	time_t	gmtTime = Route::convertTimeToGMT(stat_buff.st_mtime);
	return (Route::formatTimeString(gmtTime));
}

std::string	Route::getFileSize(const std::string &filename) {
	struct stat	stat_buff;
	stat(filename.c_str(), &stat_buff);
	return (toString(stat_buff.st_size));
}

time_t	Route::convertTimeToGMT(time_t t) {
	struct tm	*gmtTime = gmtime(&t);
	return (mktime(gmtTime));
}


std::string	Route::formatTimeString(time_t	time) {
	char	buffer[80];
	std::strftime(buffer, sizeof(buffer), "%c", localtime(&time));
	std::string strTime(buffer);
	strTime += " GMT";
	return (strTime);
}

std::string	Route::getCurrentTimeInGMT(void) {
	time_t	now = Route::convertTimeToGMT(time(0));
	return (Route::formatTimeString(now));
}