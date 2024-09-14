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
//verificar permissão de execução
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
            && !(file.st_mode & S_IRGRP))
            || (!(file.st_mode & S_IXUSR)
            && !(file.st_mode & S_IXGRP))) {
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
        path += pathPiece;;
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

std::vector<char> Handler::ReadRegularFile(std::ifstream *file) {
    int size = 100;
    char buff[size];
    std::vector<char> vector;

    do {
        memset(buff, '\0', sizeof(char) * size);
        file->read(reinterpret_cast<char*>(&buff), sizeof(buff));
        std::streamsize bytesRead = file->gcount();
        vector.insert(vector.end(), buff, buff + bytesRead);
    } while (file->good());
    return vector;
}