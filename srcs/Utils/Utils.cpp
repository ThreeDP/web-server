#include "Utils.hpp"

void	removeChar(std::string& str, char c) {
    size_t pos = 0;
    while ((pos = str.find(c, pos)) != std::string::npos) {
        str.erase(pos, 1);
    }
}

void	fixPath(std::string& str){
	removeChar(str, '^');
	removeChar(str, ';');
}

bool endsWithSemicolon(const std::string& str) {
    return !str.empty() && str[str.size() - 1] == ';';
}

std::string Utils::GetFileExtension(std::string path) {
    std::size_t dot = path.find_last_of('.');
    if (dot == std::string::npos)
        return "";
    return path.substr(dot);
}

// Statics Functions
std::string Utils::getActualDir(std::string path) {
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

std::string	Utils::getLastModifiedOfFile(const std::string &filename) {
	struct stat stat_buff;
	stat(filename.c_str(), &stat_buff);
	time_t	gmtTime = Utils::convertTimeToGMT(stat_buff.st_mtime);
	return (Utils::formatTimeString(gmtTime));
}

std::string	Utils::getFileSize(const std::string &filename) {
	struct stat	stat_buff;
	stat(filename.c_str(), &stat_buff);
	return (toString(stat_buff.st_size));
}

time_t	Utils::convertTimeToGMT(time_t t) {
	struct tm	*gmtTime = gmtime(&t);
	return (mktime(gmtTime));
}

std::string	Utils::formatTimeString(time_t	time) {
	char	buffer[80];
	std::strftime(buffer, sizeof(buffer), "%c", localtime(&time));
	std::string strTime(buffer);
	strTime += " GMT";
	return (strTime);
}

std::string	Utils::getCurrentTimeInGMT(void) {
	time_t	now = Utils::convertTimeToGMT(time(0));
	return (Utils::formatTimeString(now));
}

void   Utils::checkPathEnd(std::string &path, std::string append) {
    if (!path.empty() && path[path.size() - 1] == '/')
        path += append;
    else if (!append.empty() && append[0] == '/'){
        path += append;
    } else {
        path += "/"; 
        path += append;
    }
}

std::string Utils::SanitizePath(std::string root, std::string path) {
    std::stringstream response;
    
    if (root.length() > 0 && root[root.length() - 1] == '/' && path.length() > 0 && path[0] == '/')
        path = path.substr(1);
    else if (root.length() > 0 && root[root.length() - 1] != '/' && path.length() > 0 && path[0] != '/')
        path = "/" + path;
    response << root << path;
    return response.str();
}