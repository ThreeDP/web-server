# include "Route.hpp"

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

AHttpResponse    *Route::ProcessRoute(HttpRequest &httpReq) {
    std::string body;
    std::string newP = this->_directory;
    httpReq._path = this->_directory + httpReq._path;

    return this->DetermineOutputFile(httpReq);
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

mode_t Route::CatFileMode(std::string &path, int &statusCode) {
    struct stat sb;
    std::string newPath;
    bool        error = false;

    memset(&sb, 0, sizeof(struct stat));
    newPath = path;
    while (1) {
        if (stat(newPath.c_str(), &sb) == -1 && error == false) {
            statusCode = 404;
            this->pathReset(newPath);
            newPath += this->_error_page[statusCode];
            error = true;
            continue ;
        } else {
            path = newPath;
        }
        break;
    }
    return sb.st_mode;
}

bool Route::FindFilePattern(std::string &path, std::set<std::string> *dirs) {
    std::set<std::string>::iterator it = this->_index.begin();
    for ( ; it != this->_index.end(); ++it) {
        std::set<std::string>::iterator i = dirs->find(*it);
        if (i != dirs->end()) {
            Utils::checkPathEnd(path, *it);
            delete dirs;
            return true;
        }
    }
    return false;
}

std::string Route::GenerateAutoindex(std::vector<struct dirent *> dirs, std::string path) {
    std::stringstream   body;

    std::string actualDir = Utils::getActualDir(path);
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
        body << "\t\t" << Utils::getLastModifiedOfFile(filePath) << " " << Utils::getFileSize(filePath) << "\r\n";
    }
    body << "\t</prev>\r\n";
    body << "\t<hr>\r\n";
    body << "</body>\r\n";
    body << "</html>";
    return body.str();
}

AHttpResponse *Route::DetermineOutputFile(HttpRequest &httpReq) {
    std::stringstream   body;
    int                 statusCode = 200;
    bool                exitCheck = false;
    AHttpResponse       *res = NULL;

    this->_stage = R_REQUEST;
    std::cout << *this; 
    while (1) {
        std::set<std::string> *dirNames = NULL;
        std::vector<struct dirent *> dirs;
        switch (CatFileMode(httpReq._path, statusCode) & S_IFMT) {
        case S_IFDIR:
            dirNames = this->CatDirectorysFiles(httpReq._path, dirs);
            if (dirNames != NULL && !this->FindFilePattern(httpReq._path, dirNames)) {
                if ((res = this->checkFilePermission(httpReq, statusCode)))
                    exitCheck = true;
                if (statusCode == 200) {
                    body << GenerateAutoindex(dirs, httpReq._path);
                    delete dirNames;
                    exitCheck = true;
                    res = new Response200OK("text/html", body.str());
                }
            }
            break;
        case S_IFREG:
            if ((res = this->checkFilePermission(httpReq, statusCode)))
                exitCheck = true;
            if (statusCode == 200) {
                body << this->ReturnFileRequest(httpReq._path);
                exitCheck = true;
                res = new Response200OK("text/html", body.str());
            }
            break;
        default:
            exitCheck = true;
            std::stringstream code;
            code << statusCode;
            res = new AHttpResponse(code.str(), "text/html");
            break;
        }
        if (exitCheck)
            break;
    }
    return res;
}

AHttpResponse *Route::checkFilePermission(HttpRequest &httpReq, int &statusCode) {
    struct stat sb;

    memset(&sb, 0, sizeof(struct stat));
    stat(httpReq._path.c_str(), &sb);
    if (httpReq._method == "GET" && !(sb.st_mode & S_IRUSR)) {
        statusCode = 403;
        std::map<int, std::string>::iterator it = this->_error_page.find(statusCode);
        if (it == this->_error_page.end()) {
            std::stringstream code;
            code << statusCode;
            return new AHttpResponse(code.str(), "text/html");
        } else {
            this->pathReset(httpReq._path);
            Utils::checkPathEnd(httpReq._path, it->second);
        }
        return NULL;
    }
    statusCode = 200;
    return NULL;
}

void    Route::pathReset(std::string &path) {
    path = this->_directory;
    path += this->_route_name; 
}

// Geters
std::set<std::string>    *Route::GetAllowMethods(void) {
    return this->_allow_methods;
}

std::string Route::GetRedirectPath(void) {
    return this->_redirectPath;
}

std::map<int, std::string>  Route::GetErrorPage(){
    return this->_error_page;
}

std::string                 Route::GetErrorPage(int statusCode) {
    std::map<int, std::string>::iterator it = _error_page.find(statusCode);
    if (it != _error_page.end()) {
        return _error_page[statusCode];
    }
    return "";
}

bool                        Route::IsAllowMethod(std::string method) {
    std::set<std::string>::iterator it = _allow_methods->find(method);
    if (it != _allow_methods->end()) {
        return true;
    }
    return false;
}

int                         Route::GetLimitClientBodySize(void) const {
    return _limit_client_body_size;
}

std::string                 Route::GetRoot(void) const {
    return _directory;
}

// Seters
void    Route::SetAllowMethods(std::set<std::string> *methods) {
    this->_allow_methods = methods;
}

void    Route::SetRedirectPath(std::string redirect) {
    this->_redirectPath = redirect;
}


void		Route::SetErrorPageRoute(std::pair<int, std::string> error_page){
    this->_error_page.insert(std::make_pair(error_page.first, error_page.second));
}

std::string                 Route::GetRouteName(void) const {
    return this->_route_name;
}

std::set<std::string>       Route::GetFilesForIndex(void) const {
    return _index;
}

// Base Methods
Route::Route(CommonParameters *server, std::string server_name)  : 
    _allow_methods(server->GetDefaultAllowMethods()),
    _error_page(std::map<int, std::string>()),
    _limit_client_body_size(2048),
    _directory(server->GetRoot()),
    _autoIndex(server->GetAutoIndex()),
    _index(server->GetIndex()),
    _stage(R_START)
{
    std::map<std::string, std::string>::iterator it = server->GetReWrites().find(server_name);
    if (it != server->GetReWrites().end())
        this->_redirectPath = server->GetReWrites()[server_name];
    this->_route_name = server_name;
    std::cout << *this;
    (void)this->_limit_client_body_size;
}

Route::Route(IServer *server, std::string server_name)  : 
    _allow_methods(server->GetDefaultAllowMethods()),
    _error_page(server->GetDefaultErrorPage()),
    _limit_client_body_size(2048),
    _directory(server->GetRoot()),
    _autoIndex(server->GetAutoIndex()),
    _index(server->GetIndex()),
    _stage(R_START)
{
    std::map<std::string, std::string>::iterator it = server->GetReWrites().find(server_name);
    if (it != server->GetReWrites().end())
        this->_redirectPath = server->GetReWrites()[server_name];
    this->_route_name = server_name;
    std::cout << *this;
    (void)this->_limit_client_body_size;
}

std::ostream &operator<<(std::ostream &os, Route const &route) {
    switch (route._stage) {
    case R_START:
        os << HYEL "[ Create Route: " << route.GetRouteName() << " ]" reset << std::endl;
        break;
    case R_REQUEST:
        os << HYEL "[ Request sent to route: " << route.GetRouteName() << " ]" reset << std::endl; 
        break;
    default:
        os << "Error";
    }
	return (os);
}