# include "Response201Created.hpp"

Response201Created::Response201Created(std::string extension, std::string location, std::string body) :
    AHttpResponse("201", extension, body) {
        this->_headers["Location:"] = location;
}


Response201Created::Response201Created(std::string extension, std::string location) :
    AHttpResponse("201", extension) {
        this->_headers["Location:"] = location;
}

Response201Created::~Response201Created(void) {
    
}