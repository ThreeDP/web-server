# include "ResponseOK.hpp"

// Base Methods

ResponseOK::ResponseOK(std::string extension, std::string body) : 
	AHttpResponse("200", extension, body) {

}

ResponseOK::ResponseOK(std::string extension) : 
	AHttpResponse("200", extension) {

}

ResponseOK::~ResponseOK(void) {

}