# include "Handler.hpp"

std::ifstream *Handler::OpenFile(std::string path) {
    std::ifstream *file = new std::ifstream(path.c_str());
    if (file->is_open())
        return file;
    delete file;
    return NULL;
}

DIR *Handler::OpenDirectory(std::string path) {
    return opendir(path.c_str());
}

bool Handler::FileExist(std::string path) {
    struct stat file;

    memset(&file, 0, sizeof(struct stat));
    if (stat(path.c_str(), &file) == 0) {
        return true;
    }
    return false;
}

bool Handler::FileIsDirectory(std::string path) {
    struct stat file;

    memset(&file, 0, sizeof(struct stat));
    if (stat(path.c_str(), &file) == 0
        && file.st_mode & S_IFDIR) {
        return true;
    }
    return false;
}

bool Handler::FileIsRegular(std::string path) {
    struct stat file;

    memset(&file, 0, sizeof(struct stat));
    if (stat(path.c_str(), &file) == 0
        && file.st_mode & S_IFREG) {
        return true;
    }
    return false;
}

bool Handler::IsAllowToGetFile(std::string path) {
    struct stat file;
    std::stringstream ss(path);
    std::string pathPiece;

    path = "";
    while (std::getline(ss, pathPiece, '/'))
    {
        path += pathPiece;
        memset(&file, 0, sizeof(struct stat));
        if (this->FileIsDirectory(path))
            path += "/";
        if (stat(path.c_str(), &file) != 0
            || (!(file.st_mode & S_IRUSR)
            && !(file.st_mode & S_IRGRP))) {
            return false;
        }
    }
    return true;
}

bool Handler::PathExist(std::string path) {
    struct stat file;
    std::stringstream ss(path);
    std::string pathPiece;

    path = "";
    while (std::getline(ss, pathPiece, '/'))
    {
        path += pathPiece;
        memset(&file, 0, sizeof(struct stat));
        if (this->FileIsDirectory(path))
            path += "/";
        if (stat(path.c_str(), &file) != 0) {
            return false;
        }
    }
    return true;
}

bool Handler::IsAllowToRunScript(std::string path) {
    struct stat file;

    memset(&file, 0, sizeof(struct stat));
    if (stat(path.c_str(), &file) == 0
        && file.st_mode & S_IXUSR) {
        return true;
    }
    return false;
}

std::set<std::string> Handler::ReadDirectory(DIR *directory) {
    std::set<std::string>	dirnames;;
    struct dirent* entry = NULL;
    while ((entry = readdir(directory)) != NULL) {
        dirnames.insert(entry->d_name);
    }
    return dirnames;
}

std::string Handler::ReadRegularFile(std::ifstream *file) {
    std::stringstream payload;
    std::string line;
    while (std::getline(*file, line, '\n')) {
        payload << line;
    }
    file->close();
    return payload.str();
}