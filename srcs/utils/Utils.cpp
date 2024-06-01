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