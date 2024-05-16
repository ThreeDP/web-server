#ifndef PARSER_HPP
#define PARSER_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>

class Parser
{
	private:
		std::map<std::string, std::string>	parseRewrites(char * configFile);
		std::vector<std::string>			parseAllowMethods(char * configFile);
	
	public:
		Parser(void);
		Parser& operator=(const Parser& other);
		~Parser();

		std::map<std::string, std::string>	test_parseRewrites(char *file);
		std::vector<std::string>			test_parseAllowMethods(char *file);
};

#endif 