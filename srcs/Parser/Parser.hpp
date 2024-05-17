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
		std::pair<std::string, std::string>	_parseRewrites(std::istringstream &iss);
		std::vector<std::string>			_parseAllowMethods(std::istringstream &iss);
		void								_parserServerName(std::istringstream &iss, Http &http, Server *server);
	
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