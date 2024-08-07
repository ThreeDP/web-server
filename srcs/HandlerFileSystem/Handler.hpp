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

		std::vector<struct dirent *> 	*ReadDirectory(DIR *directory);
		std::string						ReadRegularFile(std::ifstream *file);

		bool                            PathExist(std::string path);
};

#endif