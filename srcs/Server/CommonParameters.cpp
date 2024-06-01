# include "CommonParameters.hpp"

// Seters
void		CommonParameters::SetServerName(std::string server_name) {
	this->_server_names.push_back(server_name);
}

// Geters
std::set<std::string>	&CommonParameters::GetDefaultAllowMethods(void) {
	return this->_default_allow_methods;
}

std::map<int, std::string>	&CommonParameters::GetDefaultErrorPage(void) {
	return this->_default_error_page;
}

int CommonParameters::GetLimitClientBodySize(void) {
	return this->_limit_client_body_size;
}

std::map<std::string, std::string>	&CommonParameters::GetReWrites(void) {
	return this->_rewrites;
}

std::string CommonParameters::GetRoot(void) {
	return this->_root;
}

std::set<std::string> &CommonParameters::GetIndex(void) {
	return this->_index;
}

bool CommonParameters::GetAutoIndex(void) {
	return this->_autoindex;
}

// Base Methods
CommonParameters::CommonParameters(void) : 
_listen_host("127.0.0.1"),
_listen_port(80),
_limit_client_body_size(2048),
_root("."),
_autoindex(false) {
	this->_server_names.push_back("localhost");
	this->_default_allow_methods.insert("GET");

}

CommonParameters::CommonParameters(std::vector<std::string> serv, unsigned short port) : 
_listen_host("127.0.0.1"),
_listen_port(port),
_server_names(serv),
_limit_client_body_size(2048),
_root("."),
_autoindex(false) {
	this->_default_allow_methods.insert("GET");
}