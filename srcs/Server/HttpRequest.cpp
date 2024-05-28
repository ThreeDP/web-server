# include "HttpRequest.hpp"

void    HttpRequest::ParserRequest(std::string request) {
    int                 i = 0;
    std::string         line;
    std::stringstream   srequest(request);

    while (std::getline(srequest, line)) {
        ++i;
        std::stringstream swords(line);
        if (i == 1) {
            std::getline(swords, this->_method, ' ');
            std::getline(swords, this->_path, ' ');
            std::getline(swords, this->_HTTPVersion, ' ');
            continue;
        }
        std::string key;
        std::string value;
        std::getline(swords, key, ' ');
        std::getline(swords, value, '\r');
        this->_payload[key] = value;
    }
}

// Geters

std::string         HttpRequest::GetMethod(void) const {
    return this->_method;
}

std::string         HttpRequest::GetPath(void) const {
    return this->_path;
}

std::string         HttpRequest::GetHTTPVersion(void) const {
    return this->_HTTPVersion;
}

std::string         HttpRequest::GetBody(void) const {
    return this->_body;
}

// Base Methods
HttpRequest::HttpRequest(void) {

}

HttpRequest::~HttpRequest(void) {

}