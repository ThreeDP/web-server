# include "Http.hpp"

Server *Http::GetServer(std::string server) {
    std::map<std::string, Server *>::iterator it = this->servers.find(server);
    if (it != this->servers.end()) {
        return it->second;
    }
    return NULL;
}

void Http::SetServer(std::string serverName, Server *server) {
    this->servers[serverName] = server;
}