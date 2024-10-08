#ifndef UTILS_HPP
# define UTILS_HPP

# include "define.hpp"
#include <iostream>
# include <sstream>
#include <sys/stat.h> // Include for stat
#include <string> 
#include <sys/stat.h>  // Include for stat
#include <ctime>       // Include for strftime, localtime
#include <string>      // Include for std::string
#include <iostream>    // Include for std::cerr
#include <cstdio>


void	removeChar(std::string &str, char c);
void	fixPath(std::string &str);
bool    endsWithSemicolon(const std::string &str);

class Utils {
	
	public:
        static bool                 SanitizeOneMethod(std::string method);
        static bool 				SanitizeMethods(std::set<std::string> methods);
        static std::string          SanitizePath(std::string root, std::string path);
		static std::string          getActualDir(std::string path);
        static std::string	        getLastModifiedOfFile(const std::string &filename);
        static std::string          getFileSize(const std::string &filename);
        static time_t	            convertTimeToGMT(time_t t);
        static std::string	        formatTimeString(time_t	time);
        static std::string          getCurrentTimeInGMT(void);
        static void                 checkPathEnd(std::string &path, std::string append);
        static std::string          GetFileExtension(std::string path);
        static std::string          VectorToString(std::vector<std::string> words);
        static void                 DeleteEnvp(char **envp);
};

template<typename T>
std::string	toString(const T& value) {
	std::ostringstream oss;
	oss << value;
	return (oss.str());
}

#endif