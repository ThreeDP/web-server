# include "HttpRequest.hpp"

void    HttpRequest::ParserRequest(std::string request) {
    int                 i = 0;
    std::string         line;
    std::stringstream   srequest(request);
    bool                isBody = false;

    while (std::getline(srequest, line)) {
        ++i;
        std::stringstream swords(line);
        if (i == 1) {
            std::getline(swords, this->_method, ' ');
            std::getline(swords, this->_path, ' ');
            std::getline(swords, this->_HTTPVersion, '\n');
            this->_HTTPVersion.pop_back();
            continue;
        }
        if (isBody || std::strncmp(swords.str().c_str(), "\r\n", 1) == 0) {
            if (isBody) {
                std::getline(swords, this->_body, '\n');
                this->_body.pop_back();
                break;
            }
            isBody = true;
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

const std::map<std::string, std::string>  HttpRequest::GetHeaders(void) const {
    return this->_payload;
}

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

bool HttpRequest::operator==(const HttpRequest &other) {
    bool res = _method == other.GetMethod() &&
        _path == other.GetPath() &&
        _HTTPVersion == other.GetHTTPVersion();
    std::map<std::string, std::string> headers = other.GetHeaders();
    std::map<std::string, std::string>::iterator ithis = _payload.begin();
    std::map<std::string, std::string>::iterator iother = headers.begin();
    for ( ; ithis != _payload.end() && iother != headers.end(); ) {
        res = res && ithis->first == iother->first;
        res = res && ithis->second == iother->second;
        ++ithis;
        ++iother; 
    }
    res = res && _body == other.GetBody();
    return res;
}

std::ostream &operator<<(std::ostream &os, HttpRequest const &request) {
    os << request.GetMethod() << " " << request.GetPath() << " " << request.GetHTTPVersion() << "\r\n";
    std::map<std::string, std::string> headers = request.GetHeaders(); 
    std::map<std::string, std::string>::iterator it = headers.begin();
    for (it; it != headers.end(); ++it) {
        os << it->first << " " << it->second << "\r\n";
    }
    os << "\r\n";
    os << request.GetBody();
    return os;
}