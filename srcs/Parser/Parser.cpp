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

std::pair<std::string, std::string> Parser::_parserRewrites(std::istringstream &iss){

    std::pair<std::string, std::string>	rewrites;
	std::string							location;
	std::string							rewrite;

	std::cout << "estou aqui: " << std::endl;	

	iss >> location >> rewrite;

	std::cout << "path: " << location << " new_path: " << rewrite << std::endl;	

	for (std::string::iterator it = location.begin(); it != location.end(); ++it){
		if (*it == '^'){
			location.erase(it);
			--it;
		}
	}
	rewrites =  std::make_pair(location, rewrite);
	return (rewrites);
}

std::string Parser::_parserAlocationRewrite(std::istringstream &iss){
	std::string token;
	std::string	path;

	iss >> token >> path;
	std::cout << "test: " << token << "-> " << path << std::endl;
	if (token.empty()){
		std::cout << "cade a except" << std::endl;
		throw Except("Rewrite without argument.");
	}
	if (path.empty()){
		std::cout << "token rewrite: "<< token << std::endl;
		return std::string(token);
	} else{
		std::cout << "path rewrite: "<< token << std::endl;

		return std::string(path);
	}
}

std::set<std::string>    *Parser::_parserAllowMethods(std::istringstream &iss) {
    
    std::set<std::string>	*allow_methods = new std::set<std::string>;
	std::string			methodsLine;
	std::getline(iss, 	methodsLine, ';');
	std::istringstream	sstream(methodsLine);
	std::string			method;

	while (sstream >> method){
		allow_methods->insert(method);
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
		server->SetServerName(token);
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
				server->routes[actualRoute] = new Route(server, actualRoute);
				inLocation = true;
			} else if (token == "}") {
				inLocation = false;
			} else if (token == "allow_methods" && inLocation) {
				std::set<std::string> *alm = this->_parserAllowMethods(iss);
				server->routes[actualRoute]->SetAllowMethods(alm);
			} else if (token == "rewrite" && inLocation) {
				std::string inloc = this->_parserAlocationRewrite(iss);
				server->routes[actualRoute]->SetRedirectPath(inloc);
			} else if (token == "rewrite" && !inLocation) {
				std::pair<std::string, std::string> inloc = this->_parserRewrites(iss);
				server->SetRewrites(inloc);
			}
			break;
		}
	}
}

unsigned short	Parser::_parserServerPort(std::istringstream &iss){
	std::string		token;
	unsigned short	port = 0;

	iss >> token >> port;
	return port;
}

std::pair<std::string, std::string> Parser::_parserErrorPage(std::istringstream &iss){

    std::pair<std::string, std::string>	map;
	std::string							value;
	std::string							key;
	std::string							error_page;

	iss >> error_page >> key >> value;
	map =  std::make_pair(key, value);
	return (map);
}

unsigned long Parser::_parserClientMaxMessage(std::istringstream &iss){
	std::string		token;
	unsigned long	size;
	char			unit;

	iss >> token >> size >> unit;
	switch (unit){
		case 'K': size = 1024; break;
		case 'M': size = 1048576; break;
		case 'G': size = 1073741824; break;
	}
	return (size);
}
