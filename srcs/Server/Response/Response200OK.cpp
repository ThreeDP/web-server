# include "Response200OK.hpp"

// Base Methods

Response200OK::Response200OK(std::string extension, std::string body) : 
	AHttpResponse("200", extension, body) {

}

Response200OK::Response200OK(std::string extension) : 
	AHttpResponse("200", extension) {

}

Response200OK::~Response200OK(void) {

}