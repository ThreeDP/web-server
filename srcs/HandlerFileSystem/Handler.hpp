#ifndef __HANDLER_HPP__
# define __HANDLER_HPP__

#include "IHandler.hpp"

class Handler : public IHandler {

	public:
		Handler(void) { }

		std::ifstream *OpenFile(std::string path) {
			std::ifstream *file = new std::ifstream(path.c_str());
			if (file->is_open())
				return file;
			delete file;
			return NULL;
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
				&& file.st_mode & S_IFREG) {
				return true;
			}
			return false;
		}

		bool IsAllowToGetFile(std::string path, std::string root) {
			struct stat file;
			std::stringstream ss(path);
			std::string pathPiece;
			std::string p = root;

			while (std::getline(ss, pathPiece, '/'))
			{
				p += pathPiece;
				memset(&file, 0, sizeof(struct stat));
				if (this->FileIsDirectory(p))
					p += "/";
				if (stat(p.c_str(), &file) != 0
					|| (!(file.st_mode & S_IRUSR)
					&& !(file.st_mode & S_IRGRP))) {
					return false;
				}
			}
			return true;
		}

		bool IsAllowToRunScript(std::string path) {
			struct stat file;

			memset(&file, 0, sizeof(struct stat));
			if (stat(path.c_str(), &file) == 0
				&& file.st_mode & S_IXUSR) {
				return true;
			}
			return false;
		}

		std::set<std::string> *ReadDirectory(DIR *directory) {
			std::set<std::string>		*dirnames = new std::set<std::string>();
			struct dirent* entry;
			while ((entry = readdir(directory)) != NULL) {
				std::string fileName = entry->d_name;
				dirnames->insert(fileName);
			}
			return dirnames;
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