#ifndef __ICLIENT_HPP__
# define __ICLIENT_HPP__

#include "EHttpStatusCode.hpp"

class IClient {
    public:
        virtual ~IClient(void) {}
        virtual void SetPid(pid_t pid) = 0;
        virtual pid_t GetPid(void) = 0;
        virtual bool CreatePair(void) = 0;
        virtual bool CreatePair2(void) = 0;
        virtual int GetWRPipe(void) = 0;
        virtual int GetWRPipe2(void) = 0;
        virtual int GetRDPipe(void) = 0;
        virtual int GetRDPipe2(void) = 0;
        virtual void SetStatus(HttpStatusCode::Code code) = 0;
        virtual HttpStatusCode::Code GetStatus(void) = 0;
};

#endif