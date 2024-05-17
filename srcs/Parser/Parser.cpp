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

std::pair<std::string, std::string> Parser::_parseRewrites(std::istringstream &iss){

    std::pair<std::string, std::string>	rewrites;
	std::string							location;
	std::string							rewrite;

	iss >> location >> rewrite;
	for (std::string::iterator it = location.begin(); it != location.end(); ++it){
		if (*it == '^'){
			location.erase(it);
			--it;
		}
	}
	rewrites =  std::make_pair(location, rewrite);
	return (rewrites);
}

std::vector<std::string>    Parser::_parseAllowMethods(std::istringstream &iss) {
    
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

void	Parser::_parserServerName(std::istringstream &iss, Http &http, Server *server) {
	std::string token;

	while ((iss >> token) && token != ";") {
		bool endWith = false;
		if (!token.empty() && token[token.size() - 1] == ';') {
			token = token.substr(0, token.size() - 1);
			endWith = true;
		}
		http.SetServer(token, server);
		server->server_names.push_back(token);
		if (endWith == true)
			break;
	}
}

void	Parser::ParserServer(Http &http) {
	std::string					line;
	bool						inLocation = false;
	std::string					actualRoute;						
	Server						*server;

	while (std::getline(this->_fd, line)) {
		std::istringstream	iss(line);
		std::string			token;
	
		while (iss >> token) {
			if (token == "server") {
				server = new Server();
				if ((iss >> token) && token != "{") {
					throw (Except("Error: Server expected '{'."));
				}
			} else if (token == "server_name") {
				this->_parserServerName(iss, http, server);
			} else if (token == "location" && (iss >> token)) {
				actualRoute = token;
				server->routes[actualRoute] = new Route();
				inLocation = true;
			} else if (token == "}") {
				inLocation = false;
			} else if (token == "allow_methods" && inLocation) {
				server->routes[actualRoute]->SetAllowMethods(this->_parseAllowMethods(iss));
			} else if (token == "rewrite" && inLocation) {
				server->routes[actualRoute]->SetRedirectPath(this->_parseRewrites(iss));
			}
			break;
		}
	}
}

// std::map<std::string, std::string>	Parser::test_parseRewrites(void){
//     return parseRewrites();
// }

// std::vector<std::string> Parser::test_parseAllowMethods(void){
//     return parseAllowMethods();
// }