# include "RouteResponse.hpp"

// Route Response Methods

std::string RouteResponse::defaultErrorPage(int statusCode) {
	std::stringstream sb;

	sb << "<html data-lt-installed=\"true\">";
	sb << "<head><title>";
	sb << statusCode << " " << this->_status[statusCode];
	sb << "</title></head>";
	sb << "<body>";
	sb << "<center><h1>";
	sb << statusCode << " " << this->_status[statusCode];
	sb << "</h1></center>";
	sb << "<hr><center>";
	sb << "RavyServer";
	sb << "/" << "1.27.0" << "</center>";
	sb << "</body></html>";
	return sb.str();
}

// Geters

std::string RouteResponse::getBody(void) const {
	return this->_body; 
}

std::string RouteResponse::getStatusCode(void) const {
	std::stringstream str;
	str << this->_statusCode;
	return str.str();
}

std::string RouteResponse::getStatusName(void) {
	return this->_status[this->_statusCode];
}

std::string RouteResponse::getContentType(void) const {
	return this->_contenttype;
}

// Base Methods

RouteResponse::RouteResponse(std::string b, int sc) :
_body(b), _statusCode(sc), _contenttype("text/html") {
	_status[200] = "OK";
	_status[201] = "Created";
	_status[202] = "Accepted";
	_status[204] = "No Content";
	_status[300] = "Multiple Choice";
	_status[301] = "Moved Permanently";
	_status[302] = "Found";
	_status[400] = "Bad Request";
	_status[401] = "Unauthorized";
	_status[403] = "Forbidden";
	_status[404] = "Not Found";
	_status[405] = "Method Not Allowed";
	_status[413] = "Request Entity Too Large";
	_status[415] = "Unsupported Media Type";
	_status[500] = "Internal Server Error";
	_status[502] = "Bad Gateway";
	_status[504] = "Gateway Timeout";
	_status[505] = "HTTP Version Not Supported";
}

RouteResponse::RouteResponse(int sc) :
_statusCode(sc), _contenttype("text/html") {
	_status[200] = "OK";
	_status[201] = "Created";
	_status[202] = "Accepted";
	_status[204] = "No Content";
	_status[300] = "Multiple Choice";
	_status[301] = "Moved Permanently";
	_status[302] = "Found";
	_status[400] = "Bad Request";
	_status[401] = "Unauthorized";
	_status[403] = "Forbidden";
	_status[404] = "Not Found";
	_status[405] = "Method Not Allowed";
	_status[413] = "Request Entity Too Large";
	_status[415] = "Unsupported Media Type";
	_status[500] = "Internal Server Error";
	_status[502] = "Bad Gateway";
	_status[504] = "Gateway Timeout";
	_status[505] = "HTTP Version Not Supported";
	this->_body = this->defaultErrorPage(sc);
}