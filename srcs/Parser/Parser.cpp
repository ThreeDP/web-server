#include "Parser.hpp"

Parser::Parser(void){}

Parser& Parser::operator=(const Parser& other){
    if (this == &other){return *this;}
    return *this;
}

Parser::~Parser(){}

std::map<std::string, std::string> Parser::parseRewrites(char *configFile){

    std::map<std::string, std::string>	rewrites;
	std::ifstream						file(configFile);
	std::string							line;
	std::string							word;
	std::string							location;
	std::string							rewrite;
	bool								inLocation = false;

	if(!file.is_open()){
		std::cerr << "Error opening configuration file." << std::endl;
		return rewrites;
	}

	while(std::getline(file, line)){

		std::istringstream	iss(line);
		std::string			token;

		while(iss >> token){
			if(token == "location"){
				inLocation = true;
			} else if (token == "}"){
				inLocation = false;
			}else if (token == "rewrite" && inLocation){
					iss >> location >> rewrite;
					for (std::string::iterator it = location.begin(); it != location.end(); ++it){
						if (*it == '^'){
							location.erase(it);
							--it;
						}
					}
					rewrites[location] = rewrite;
			}
		}
	}
	file.close();
	return (rewrites);
}

std::vector<std::string>    Parser::parseAllowMethods(char *configFile){
    
    std::vector<std::string>	allow_methods;
	bool						inLocation = false;
	std::ifstream				file(configFile);
	std::string					line;

	if(!file.is_open()){
		std::cerr << "Error opening configuration file." << std::endl;
		return allow_methods;
	}

	while (std::getline(file, line)){
		std::istringstream	iss(line);
		std::string			token;

		while(iss >> token){
			if(token == "location"){
				inLocation = true;
			} else if (token == "}"){
				inLocation = false;
			}else if (token == "allow_methods" && inLocation){
				std::string			methodsLine;
				std::getline(iss, methodsLine, ';');
				std::istringstream	sstream(methodsLine);
				std::string			method;
				while (sstream >> method){
					allow_methods.push_back(method);
				}
			}
		}
	}
	file.close();
	return allow_methods;
}


std::map<std::string, std::string>	Parser::test_parseRewrites(char *file){
    return parseRewrites(file);
}

std::vector<std::string> Parser::test_parseAllowMethods(char *file){
    return parseAllowMethods(file);
}