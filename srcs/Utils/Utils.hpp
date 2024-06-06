#ifndef UTILS_HPP
# define UTILS_HPP

# include "define.hpp"

void	removeChar(std::string& str, char c);
void	fixPath(std::string& str);
bool    endsWithSemicolon(const std::string& str);

class Utils {
	
	public:
		static std::string          getActualDir(std::string path);
        static std::string	        getLastModifiedOfFile(const std::string &filename);
        static std::string          getFileSize(const std::string &filename);
        static time_t	            convertTimeToGMT(time_t t);
        static std::string	        formatTimeString(time_t	time);
        static std::string          getCurrentTimeInGMT(void);
        static void                 checkPathEnd(std::string &path, std::string append);
};

template<typename T>
std::string	toString(const T& value) {
	std::ostringstream oss;
	oss << value;
	return (oss.str());
}

#endif