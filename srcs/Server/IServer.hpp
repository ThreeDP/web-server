#ifndef __ISERVER_HPP__
# define __ISERVER_HPP__

class IServer {
    public:
        ~IServer(void) {}

        virtual std::set<std::string>				*GetDefaultAllowMethods(void) = 0;
        virtual std::map<int, std::string>			&GetDefaultErrorPage(void) = 0;
        virtual int 								GetLimitClientBodySize(void) = 0;
        virtual std::map<std::string, std::string>	&GetReWrites(void) = 0;
        virtual std::string 						GetRoot(void) = 0;
        virtual std::set<std::string> 				&GetIndex(void) = 0;
        virtual bool 								GetAutoIndex(void) = 0;
        virtual std::vector<std::string>			GetServerNames(void) = 0;
};

#endif 