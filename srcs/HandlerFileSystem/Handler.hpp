#ifndef __HANDLER_HPP__
# define __HANDLER_HPP__

#include "IHandler.hpp"

class Handler : public IHandler {
	struct stat _fileInfo;

	public:
		Handler(void) {
			memset(&this->_fileInfo, 0, sizeof(struct stat));
		}

		int OpenFile(std::string path) {
			return -1;
		}

		std::ifstream OpenFileStream(std::string path) {
			return std::ifstream(path.c_str());
		}

		DIR *OpenDirectory(std::string path) {
			return opendir(path.c_str());
		}

		bool FileExist(std::string path) {
			struct stat file;

			memset(&file, 0, sizeof(struct stat));
			if (stat(path.c_str(), &file) == 0) {
				return true;
			}
			return false;
		}

		bool FileIsDirectory(std::string path) {
			struct stat file;

			memset(&file, 0, sizeof(struct stat));
			if (stat(path.c_str(), &file) == 0
				&& file.st_mode & S_IFDIR) {
				return true;
			}
			return false;
		}

		bool FileIsRegular(std::string path) {
			struct stat file;

			memset(&file, 0, sizeof(struct stat));
			if (stat(path.c_str(), &file) == 0
				&& this->_fileInfo.st_mode & S_IFREG) {
				return true;
			}
			return false;
		}

		bool IsAllowToGetFile(std::string path) {
			struct stat file;

			memset(&file, 0, sizeof(struct stat));
			if (stat(path.c_str(), &file) == 0
				&& this->_fileInfo.st_mode & S_IRUSR) {
				return true;
			}
			return false;
		}

		bool IsAllowToRunScript(std::string path) {
			struct stat file;

			memset(&file, 0, sizeof(struct stat));
			if (stat(path.c_str(), &file) == 0
				&& this->_fileInfo.st_mode & S_IXUSR) {
				return true;
			}
			return false;
		}

		std::set<std::string> ReadDirectory(DIR *directory) {
			std::set<std::string>		*dirnames = new std::set<std::string>();
			struct dirent* entry;
			while ((entry = readdir(directory)) != NULL) {
				std::string fileName = entry->d_name;
				dirnames->insert(fileName);
			}
		}

		std::string ReadRegularFile(std::ifstream file) {
			std::stringstream payload;
			std::string line;
			while (std::getline(file, line)) {
				payload << line;
			}
			file.close();
			return payload.str();
		}
};

#endif