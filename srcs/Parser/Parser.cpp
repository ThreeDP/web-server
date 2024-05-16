<<<<<<< HEAD
#include "Parser.hpp"

Parser::Parser(char *fileName) : _fd(fileName) {
    if (!this->_fd) {
        throw (Except("Configuration File Not Found!"));
    }
}

Parser& Parser::operator=(const Parser& other){
    if (this == &other){return *this;}
    return *this;
}

Parser::~Parser(){
	this._fd.close();
}

std::map<std::string, std::string> Parser::parseRewrites(void){

    std::map<std::string, std::string>	rewrites;
	std::string							line;
	std::string							word;
	std::string							location;
	std::string							rewrite;
	bool								inLocation = false;

	if(!this._fd.is_open()){
		std::cerr << "Error opening configuration file." << std::endl;
		return rewrites;
	}

	while(std::getline(this._fd, line)){

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
	return (rewrites);
}

std::vector<std::string>    Parser::parseAllowMethods(void){
    
    std::vector<std::string>	allow_methods;
	bool						inLocation = false;
	std::string					line;

	if(!this._fd.is_open()){
		std::cerr << "Error opening configuration file." << std::endl;
		return allow_methods;
	}

	while (std::getline(this._fd, line)){
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
	return allow_methods;
}


std::map<std::string, std::string>	Parser::test_parseRewrites(void){
    return parseRewrites();
}

std::vector<std::string> Parser::test_parseAllowMethods(void){
    return parseAllowMethods();
}