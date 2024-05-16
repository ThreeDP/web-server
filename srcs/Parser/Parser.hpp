#ifndef __PARSER_HPP__
#define __PARSER_HPP__

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>

class Parser
{
	private:
		Parser(void){}
        std::ifstream _fd;
		std::map<std::string, std::string>	parseRewrites(void);
		std::vector<std::string>			parseAllowMethods(void);
	
	public:
        Parser(char *fileName);
		Parser& operator=(const Parser& other);
		~Parser();

		std::map<std::string, std::string>	test_parseRewrites(void);
		std::vector<std::string>			test_parseAllowMethods(void);

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