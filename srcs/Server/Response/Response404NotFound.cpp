# include "Response404NotFound.hpp"

Response404NotFound::Response404NotFound(std::string extension, std::string body) :
	AHttpResponse("404", extension, body) {

}

Response404NotFound::Response404NotFound(std::string extension) :
	AHttpResponse("404", extension) {

}

Response404NotFound::~Response404NotFound(void) {
	
}