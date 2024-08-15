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
	std::string							oldPath;
	std::string							newPath;


	iss >> oldPath >> newPath;
	if (oldPath.empty() || oldPath == ";" || newPath.empty() || newPath == ";" || !endsWithSemicolon(newPath)){
		throw Except("Syntax error.");
	}
	for (std::string::iterator it = oldPath.begin(); it != oldPath.end(); ++it){
		if (*it == '^'){
			oldPath.erase(it);
			--it;
		}
	}
	fixPath(oldPath);
	fixPath(newPath);
	rewrites =  std::make_pair(oldPath, newPath);
	return (rewrites);
}

std::string Parser::_parserAlocationRewrite(std::istringstream &iss){
	std::string token;
	std::string	path;

	iss >> token >> path;
	if (token.empty() || token == ";"){throw Except("Syntax error.");}
	if (path.empty()){
		if (!endsWithSemicolon(token)){throw Except("Syntax error.");}
		fixPath(token);
		return std::string(token);
	} else{
		if (!endsWithSemicolon(path)){throw Except("Syntax error.");}
		fixPath(path);
		return std::string(path);
	}
}

std::set<std::string>    *Parser::_parserAllowMethods(std::istringstream &iss) {
    
    std::set<std::string>	*allow_methods = new std::set<std::string>;
	std::string			methodsLine;
	std::getline(iss, 	methodsLine, ';');
	std::istringstream	sstream(methodsLine);
	std::string			method;
	std::string 		token;

	std::string content = iss.str();
	if (content.find(';') == std::string::npos){throw Except("Syntax error.");}
	if (methodsLine.empty() || methodsLine == ";"){
		throw Except("Syntax error.");
	}
	while (sstream >> method){
		fixPath(method);
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
			} else if (token == "error_page" && inLocation){
				_parserErrorPage(iss, server, actualRoute);
			} else if (token == "error_page" && !inLocation){
				_parserErrorPage(iss, server, "");
			} else {
				throw Except("Syntax error.");
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

void	Parser::_parserErrorPage(std::istringstream &iss, Server *server, std::string actualRoute){

    std::pair<int, std::string>			map;
	std::vector<int>					keys;
	std::string							value, line;

	if (actualRoute.empty())
		std::cout << "esta vazio" << actualRoute << std::endl;
	std::getline(iss, line);
	if (!endsWithSemicolon(line)){
		throw Except("Syntax error.");
	}

	std::istringstream lineStream(line);
	while (lineStream >> value){
		if (isdigit(value[0])){
			int num; 
			std::stringstream ss(value);
			ss >> num;
			keys.push_back(num);
		} else if (value.find(".html") != std::string::npos){
			break;
		} else {
			throw Except("Syntaxe error.");
		}
	}

	if (keys.empty() || value.empty() || value.find(".html") == std::string::npos){
		throw Except("Syntax error.");
	}

	fixPath(value);
	for (size_t i = 0; i < keys.size(); ++i){
		map = std::make_pair(keys[i], value);
		if (actualRoute.empty()){
			server->SetErrorPage(map);
		} else {
			server->routes[actualRoute]->SetErrorPage(map.second);
		}
	}
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
