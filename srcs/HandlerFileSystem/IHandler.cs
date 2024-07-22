#ifndef __IHANDLER_HPP__
# define __IHANDLER_HPP__

class IHandler {

    virtual std::set<std::string>   ReadDirectory(int fd) = 0;
    virtual int                     GetFilePermission(std::string path) = 0;
    virtual bool                    IsDirectory(std::string path) = 0;
    virtual bool                    IsFile(std::string path) = 0;
}

#endif