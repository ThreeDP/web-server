#ifndef __IHANDLER_HPP__
# define __IHANDLER_HPP__

# include <iostream>
# include <sys/stat.h>
# include <sys/types.h>
# include <dirent.h>

class IHandler {
    public:

        // virtual std::set<std::string>   ReadDirectory(int fd) = 0;
        virtual DIR                             *OpenDirectory(std::string path) = 0;
        // virtual int                         OpenFile(std::string path) = 0;
        virtual std::ifstream                   *OpenFile(std::string path) = 0;

        virtual bool                            FileExist(std::string path) = 0;
        virtual bool                            FileIsDirectory(std::string path) = 0;
        virtual bool                            FileIsRegular(std::string path) = 0;
        virtual bool                            IsAllowToGetFile(std::string path) = 0;
        virtual bool                            IsAllowToRunScript(std::string path) = 0;

        virtual std::vector<struct dirent *>    *ReadDirectory(DIR *directory) = 0;
        virtual std::string                     ReadRegularFile(std::ifstream *file) = 0;
        // virtual int                     GetFilePermission(std::string path) = 0;
        // virtual bool                    IsDirectory(std::string path) = 0;
        // virtual bool                    IsFile(std::string path) = 0;
};

#endif