#ifndef __IHANDLER_HPP__
# define __IHANDLER_HPP__

# include <iostream>
# include <sys/stat.h>

class IHandler {
    public:

        // virtual std::set<std::string>   ReadDirectory(int fd) = 0;
        virtual DIR                         *OpenDirectory(std::string path) = 0;
        virtual int                         OpenFile(std::string path) = 0;
        virtual std::ifstream               OpenFileStream(std::string path) = 0;

        virtual bool                        FileExist(std::string path) = 0;
        virtual bool                        FileIsDirectory(void) = 0;
        virtual bool                        FileIsRegular(void) = 0;
        virtual bool                        IsAllowToGetFile(void) = 0;
        virtual bool                        IsAllowToRunScript(void) = 0;

        virtual std::set<std::string>       ReadDirectory(DIR *directory) = 0;
        virtual std::string                 ReadRegularFile(std::ifstream file) = 0;
        // virtual int                     GetFilePermission(std::string path) = 0;
        // virtual bool                    IsDirectory(std::string path) = 0;
        // virtual bool                    IsFile(std::string path) = 0;
};

#endif