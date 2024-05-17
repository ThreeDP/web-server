#include "Parser.hpp"

Parser::Parser(const char *fileName) : _fd(fileName) {
    if (!this->_fd) {
        throw (Except("Configuration File Not Found!"));
    }
}

Parser& Parser::operator=(const Parser& other){
    if (this == &other){return *this;}
    return *this;
}

Parser::~Parser(){
	this->_fd.close();
}

std::pair<std::string, std::string> Parser::parseRewrites(std::istringstream &iss){

    std::pair<std::string, std::string>	rewrites;
	std::string							location;
	std::string							rewrite;

	iss >> location >> rewrite;
	for (std::string::iterator it = location.begin(); it != location.end(); ++it){
		if (*it == '^'){
			location.erase(it);
			std::cout << "Test: "<< location << " " << rewrite << std::endl;
			--it;
		}
	}
	rewrites =  std::make_pair(location, rewrite);
	return (rewrites);
}

std::vector<std::string>    Parser::parseAllowMethods(std::istringstream &iss) {
    
    std::vector<std::string>	allow_methods;
	std::string			methodsLine;
	std::getline(iss, 	methodsLine, ';');
	std::istringstream	sstream(methodsLine);
	std::string			method;

	while (sstream >> method){
		allow_methods.push_back(method);
	}
	return allow_methods;
}

std::vector<Server>		Parser::ParserServer(void) {
	std::string					line;
	bool						inLocation = false;
	std::string					actualRoute;
	std::vector<Server>			servers;
	while (std::getline(this->_fd, line)) {
		std::istringstream	iss(line);
		std::string			token;
		while (iss >> token) {
			if (token == "server") {
				servers.push_back(Server());
			} else if (token == "location" && (iss >> token)) {
				actualRoute = token;
				servers.back().routes[actualRoute] = new Route();
				inLocation = true;
			} else if (token == "}") {
				inLocation = false;
			} else if (token == "allow_methods" && inLocation) {
				servers.back().routes[actualRoute]->SetAllowMethods(this->parseAllowMethods(iss));
			} else if (token == "rewrite" && inLocation) {
				servers.back().routes[actualRoute]->SetRedirectPath(this->parseRewrites(iss));
			}
			break;
		}
	}
	return servers;
}

// std::map<std::string, std::string>	Parser::test_parseRewrites(void){
//     return parseRewrites();
// }

// std::vector<std::string> Parser::test_parseAllowMethods(void){
//     return parseAllowMethods();
// }