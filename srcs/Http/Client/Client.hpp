#ifndef __CLIENT_HPP__
# define __CLIENT_HPP__

# include "IServer.hpp"
# include <ctime>

class Client {
    time_t      _lastUpdate;

    public:
        std::vector<char>   Request;
        IServer             *Server;

    Client(void) : 
        _lastUpdate(time(NULL))
    {
        Request.clear();
        Server = NULL;
    }

    ~Client(void) {

    }

    void UpdateLastUpdate(void) {
        _lastUpdate = time(NULL);
    }

    bool CheckIfExpire(void) {
        if (time(NULL) - _lastUpdate > 60) {
            return true;
        }
        return false;
    }

};

#endif