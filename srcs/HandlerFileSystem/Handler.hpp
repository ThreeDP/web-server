#ifndef __HANDLER_HPP__
# define __HANDLER_HPP__

#include "IHandler.hpp"

class Handler : public IHandler {

	public:
		Handler(void) { }
		~Handler(void) { }

		std::ifstream					*OpenFile(std::string path);
		DIR 							*OpenDirectory(std::string path);

		bool							FileExist(std::string path);
		bool							FileIsDirectory(std::string path);
		bool							FileIsRegular(std::string path);

		bool							IsAllowToGetFile(std::string path);
		bool							IsAllowToRunScript(std::string path);

		std::set<std::string> 			ReadDirectory(DIR *directory);
		std::vector<char> 				ReadRegularFile(std::ifstream *file);

		bool                            PathExist(std::string path);
		std::pair<bool, std::string>    ReadLine(std::ifstream &file) {
			std::string			line;
			bool response = (std::getline(file, line)) ? true : false;
			std::cout << "==" << line << "==" << std::endl;
			return std::make_pair(response, line);
		}
};

#endif