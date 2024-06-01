#ifndef __PARSER_HPP__
# define __PARSER_HPP__

# include <iostream>
# include <fstream>
# include <sstream>
# include <string>
# include <vector>
# include <map>

# include "Http.hpp"

class Parser
{
	private:
		Parser(void){}
        std::ifstream _fd;
		std::pair<std::string, std::string>	_parserRewrites(std::istringstream &iss);
		std::string							_parserAlocationRewrite(std::istringstream &iss);
		std::set<std::string>				*_parserAllowMethods(std::istringstream &iss);
		void								_parserServerName(std::istringstream &iss, Http &http, Server *server);
		unsigned short						_parserServerPort(std::istringstream &iss);
		std::pair<std::string, std::string> _parserErrorPage(std::istringstream &iss);
		unsigned long						_parserClientMaxMessage(std::istringstream &iss);
	
	public:
        Parser(const char *fileName);
		Parser& operator=(const Parser& other);
		~Parser();

		void					ParserServer(Http &http);

        class Except: virtual public std::exception {
			protected:
			std::string error_message;
			public:
				explicit Except(const std::string& msg) : error_message(msg) {}
				virtual ~Except() throw () {}
				virtual const char* what() const throw () { return error_message.c_str();}
		};
};

#endif 