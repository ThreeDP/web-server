# include "HttpRequest.hpp"

// void    HttpRequest::ParserRequest(std::string request) {
//     int                 i = 0;
//     std::string         line;
//     std::stringstream   srequest(request);
//     bool                isBody = false;

//     while (std::getline(srequest, line)) {
//         ++i;
//         std::stringstream swords(line);
//         if (i == 1) {
//             std::getline(swords, this->_method, ' ');
//             std::getline(swords, this->_path, ' ');
//             std::getline(swords, this->_HTTPVersion, '\r');
//             continue;
//         }
//         size_t pos = this->_path.find_first_of('?');
//         if (pos != std::string::npos) {
//             _queryStrings = this->_path.substr(pos + 1, this->_path.size());
//             this->_path = this->_path.substr(0, pos);
//         }
//         bool isSlashRFirst = std::strncmp(swords.str().c_str(), "\r", 2) == 0;
//         if (isBody || isSlashRFirst) {
//             if (!isBody && isSlashRFirst) {
//                 isBody = true;
//                 continue;
//             } else if (isBody && !isSlashRFirst) {
//                 std::string body;
//                 std::getline(swords, body, '\r');
//                 this->_body += body;
//                 this->_bodySize += this->_body.length();
//                 continue;
//             } else {
//                 this->_bodySize += 2;
//                 break;
//             }
//         }
//         std::string key;
//         std::string value;
//         std::getline(swords, key, ' ');
//         std::getline(swords, value, '\r');
//         this->_payload[key] = value;
//     }
// }

void    HttpRequest::ParserRequest(std::vector<char> &request) {
    std::string str(request.begin(), request.end());
    std::cout << "'" << str << "'" << std::endl; 
    size_t firstHeaderEnd = str.find("\r\n\r\n");
    if (firstHeaderEnd == std::string::npos) {
        std::cerr << "Formato de solicitação HTTP inválido: cabeçalhos não encontrados" << std::endl;
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

    // Parse request line
    ssize_t firstSizeLine = 0;
    std::stringstream srequest(str.substr(firstSizeLine, firstHeaderEnd));
    std::string line;

    // Read the request line
    if (std::getline(srequest, line)) {
        firstSizeLine = line.size();
        std::stringstream requestLine(line);
        std::getline(requestLine, _method, ' ');
        std::getline(requestLine, _path, ' ');
        std::getline(requestLine, _HTTPVersion, '\r');
        
        // Handle query strings
        size_t pos = _path.find_first_of('?');
        if (pos != std::string::npos) {
            _queryStrings = _path.substr(pos + 1);
            _path = _path.substr(0, pos);
        }
    }

    std::stringstream headers(str.substr(firstSizeLine, firstHeaderEnd));
    while (std::getline(headers, line) && !line.empty()) {
        std::string key;
        std::string value;
        std::stringstream headerLine(line);
        if (std::getline(std::getline(headerLine, key, ':'), value)) {
            // Trim leading spaces from the value
            value.erase(0, value.find_first_not_of(" \t"));
            _payload[key + ":"] = value;
        }
    }
    
    _bodyBinary.clear();
    if (request.size() > firstHeaderEnd)
        _bodyBinary.assign(request.begin() + firstHeaderEnd, request.end());
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
    return this->_bodyBinary.size();
}

// Base Methods
HttpRequest::HttpRequest(void) :
    _method(""),
    _path(""),
    _queryStrings(""),
    _HTTPVersion(""),
    _body(""),
    _bodySize(0) {

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
    os << request.GetMethod() << " " << request.GetPath() << " (" << request.GetQueryParams() << ") " << " " << request.GetHTTPVersion() << "\r\n";
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