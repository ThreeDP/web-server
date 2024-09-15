#ifndef __CLIENT_HPP__
# define __CLIENT_HPP__

# include "IClient.hpp"
# include "IServer.hpp"
# include <ctime>

class Client : public IClient {
    time_t      _lastUpdate;

    public:
        std::vector<char>   Request;
        IServer             *Server;
        int                 cgiPair[4];
        pid_t               _pid;

        void SetPid(pid_t pid) {
            _pid = pid;
        }

        pid_t GetPid(void) {
            return _pid;
        }

        bool CreatePair(void) {
            if (pipe(&cgiPair[2]) == -1) {
                return false;
            }
            return true;
        }

        bool CreatePair2(void) {
            if (socketpair(AF_UNIX, SOCK_STREAM, 0, this->cgiPair) == -1) {
                return false;
            }
            return true;
        }

        int GetWRPipe(void) {
            return cgiPair[3];
        }

        int GetRDPipe(void) {
            return cgiPair[2];
        }

        int GetWRPipe2(void) {
            return cgiPair[1];
        }

        int GetRDPipe2(void) {
            return cgiPair[0];
        }

    Client(void) : 
        _lastUpdate(time(NULL))
    {
        memset(&this->cgiPair, '\0', sizeof(this->cgiPair));
        Request.clear();
        Server = NULL;
        _pid = -1;
    }

    ~Client(void) {

    }

    void UpdateLastUpdate(void) {
        _lastUpdate = time(NULL);
    }

    bool CheckIfExpire(void) {
        if (time(NULL) - _lastUpdate > 2) {
            return true;
        }
        return false;
    }
};

#endif