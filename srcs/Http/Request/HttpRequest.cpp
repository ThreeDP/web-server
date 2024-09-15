# include "HttpRequest.hpp"

void    HttpRequest::ParserRequest(std::vector<char> &request) {
    std::string str(request.begin(), request.end());
    size_t firstHeaderEnd = str.find("\r\n\r\n");
    if (firstHeaderEnd == std::string::npos) {
        _flag = HttpStatusCode::_BAD_REQUEST;
        return;
    }
    firstHeaderEnd += 4;

    size_t secondHeaderStart = str.find("\r\n\r\n", firstHeaderEnd);
    if (secondHeaderStart == std::string::npos) {
        secondHeaderStart = firstHeaderEnd;
    }

    if (secondHeaderStart > firstHeaderEnd) {
        firstHeaderEnd = secondHeaderStart + 4;
    }

    ssize_t firstSizeLine = 0;
    std::stringstream srequest(str.substr(firstSizeLine, firstHeaderEnd));
    std::string line;

    if (std::getline(srequest, line, '\n')) {
        firstSizeLine = line.size();
        std::stringstream requestLine(line);
        std::getline(requestLine, _method, ' ');
        std::getline(requestLine, _path, ' ');
        std::getline(requestLine, _HTTPVersion, '\r');
        
        size_t pos = _path.find_first_of('?');
        if (pos != std::string::npos) {
            _queryStrings = _path.substr(pos + 1);
            _path = _path.substr(0, pos);
        }
    }

    std::stringstream headers(str.substr(firstSizeLine, firstHeaderEnd));
    while (std::getline(headers, line, '\n')) {
        std::string key;
        std::string value;
        std::stringstream headerLine(line);
        if (std::getline(std::getline(headerLine, key, ':'), value)) {
            value.erase(0, value.find_first_not_of(" \r"));
            _payload[key + ":"] = value;
        }
    }
    _bodyBinary.clear();
    if (request.size() > firstHeaderEnd)
        _bodyBinary.assign(request.begin() + firstHeaderEnd, request.end());
    std::istringstream iss(_payload["Content-Length:"]);
    iss >> _bodySize;
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

int              HttpRequest::GetBodySize(void) const {
    return this->_bodySize;
}

// Base Methods
HttpRequest::HttpRequest(void) :
    _method(""),
    _path(""),
    _queryStrings(""),
    _HTTPVersion(""),
    _body(""),
    _bodySize(0),
    _flag(HttpStatusCode::_DO_NOTHING) {
    client = NULL;
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
    os << request.GetMethod() << " " << request.GetPath() << " QueryParams: (" << request.GetQueryParams() << ") " << " " << request.GetHTTPVersion() << "\r\n";
    std::map<std::string, std::string> headers = request.GetHeaders(); 
    std::map<std::string, std::string>::iterator it = headers.begin();
    for ( ; it != headers.end(); ++it) {
        os << it->first << " " << it->second << "\r\n";
    }
    os << "\r\n";
    os << request.GetBody();
    return os;
}

bool HttpRequest::IsCGIRequest(void) {
    std::string path = this->GetPath();
    if (path.find(".py") != std::string::npos) {
        return true;
    }
    return false;
}