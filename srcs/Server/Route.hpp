#ifndef __ROUTE_HPP__
# define __ROUTE_HPP__

# include "define.hpp"

// I/O
# include <iostream>

// Containers
# include <vector>
# include <map>

// epoll
# include <sys/epoll.h>
# include <sys/stat.h>

// socket
# include <sys/types.h>
# include <sys/socket.h>

#include <fstream>
# include <dirent.h>

# include <vector>
# include <sstream>  

# include <cstdlib>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <syslog.h>
#include <unistd.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdint.h>
// #include <time.h>

class Route {
    private:
        std::vector<std::string>            _allowMethods;
        std::map<std::string, std::string>  _redirectPath;
        std::string                         _directory;
        
    public:
        std::map<std::string, std::string> &GetRedirectPath(void);
        std::vector<std::string>    &GetAllowMethods(void);
        void    SetAllowMethods(std::vector<std::string> methods);
        void    SetRedirectPath(std::pair<std::string, std::string> redirect);

        Route() : _directory("."){}

        std::vector<std::string>     *CatDirectorysFiles(std::string path) {
            DIR                         *dir = NULL;
            std::vector<std::string>    *directorys = new std::vector<std::string>();

            dir = opendir(path.c_str());
            if (dir != NULL) {
                struct dirent* entry;

                while ((entry = readdir(dir)) != NULL) {
                    std::string d_name = entry->d_name;
                    directorys->push_back(d_name);
                }
                closedir(dir);
            }
            return directorys;
        }

        std::string    ProcessRoute(std::string path) {
            std::string body;
            std::string newP = this->_directory;
            newP += path;

            return this->DetermineOutputFile(newP);
        }

        std::string ReturnFileRequest(std::string path) {
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

        mode_t CatFileMode(std::string path) {
            struct stat sb;

            memset(&sb, 0, sizeof(struct stat));
            if (stat(path.c_str(), &sb) == -1) {
                std::cerr << "Error on check file type";
            }
            return sb.st_mode;
        }

        bool FindFilePattern(std::string &path, std::vector<std::string> *dirs) {
            std::vector<std::string>::iterator it = dirs->begin();
            for (; it != dirs->end(); ++it) {
                if (*it == "index.html") {
                    path += *it;
                    return true;
                }
            }
            return false;
        }

        std::string DetermineOutputFile(std::string path) {
            std::stringstream body;
            bool exitCheck = false;
            while (1) {
                switch (CatFileMode(path) & S_IFMT) {
                case S_IFDIR:
                    std::vector<std::string> *dirs;
                    dirs = this->CatDirectorysFiles(path);
                    if (!this->FindFilePattern(path, dirs)) {
                        // int status = this->checkFilePermission(path);
                        for (int i = 0; i < dirs->size(); i++) {
                            body << (*dirs)[i] << "\r\n";
                        }
                        delete dirs;
                        exitCheck = true;
                    }
                    break;
                case S_IFREG:
                    // int status = this->checkFilePermission(path);
                    body << this->ReturnFileRequest(path);
                    exitCheck = true;
                    break;
                default:
                    std::cout << "default" << std::endl;
                    break;
                }
                if (exitCheck)
                    break;
            }
            return body.str();
        }

        class Except: virtual public std::exception {
			protected:
			std::string error_message;
			public:
				explicit Except(const std::string& msg) : error_message(msg) {}
				virtual ~Except() throw () {}
				virtual const char* what() const throw () { return error_message.c_str();}
		};
        //Route(std::vector<std::string> methods, std::string redirect, std::string directory);
};

#endif
