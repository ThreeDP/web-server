# include "HttpResponse.hpp"

std::map<HttpStatusCode::Code, std::string> HttpResponse::_mapStatusCode;
std::map<std::string, std::string> HttpResponse::_mapTextContent;
std::set<std::string> HttpResponse::_CGIExtensions;

// Constructors
HttpResponse::HttpResponse(int num) : 
    _server("Ravy 1.0.0"),
    _HTTPVersion("HTTP/1.1"),
    _statusCode("200"),
    _statusMessage("OK"),
    _logger(NULL)
{
    _setMapStatusCode();
    _setMapTextContent();
    _setCGIExtensions();
    (void)num;
};

HttpResponse::HttpResponse(ILogger *logger) :
    _server("Ravy 1.0.0"),
    _HTTPVersion("HTTP/1.1"),
    _statusCode("200"),
    _statusMessage("OK"),
    _logger(logger)
{
    this->_headers["Content-Type:"] = this->_mapTextContent["text"];
    this->_headers["Server:"] = this->_server;
    
    if (_logger->Env()) {
        std::cerr << _logger->Log(&Logger::LogDebug, "Create HttpResponse Class: ");
        std::cerr << _logger->Log(&Logger::LogTrace, "Initial Content {\n", this->_toString(), "\n}");
    }
}

HttpResponse::~HttpResponse(void) {
    if (_logger != NULL)
        std::cerr << _logger->Log(&Logger::LogDebug, "Deleted HttpResponse Class.");
}

// Generate Response
std::string HttpResponse::_toString(void) {
    std::stringstream ss;

    ss << _HTTPVersion << " ";
    ss << _statusCode << " ";
    ss << _statusMessage << std::endl;
    std::map<std::string, std::string>::iterator it = _headers.begin();
    for ( ; it != _headers.end(); ++it) {
        ss << it->first << " " << it->second << std::endl;
    }
    ss << std::string(_body.begin(), _body.end()) << std::endl;
    return ss.str();
}

std::vector<char> HttpResponse::CreateResponse(void) {
    std::stringstream ss;
    std::stringstream bodySize;

    bodySize << this->_body.size(); // > 0) ? this->_body.size() + 2 : 0;
    this->_headers["Content-Length:"] = bodySize.str();
    this->_headers["Date:"] = Utils::getCurrentTimeInGMT();
    ss << this->_HTTPVersion << " " << this->_statusCode << " " << this->_statusMessage << "\r\n";
    std::map<std::string, std::string>::iterator it = this->_headers.begin();
    for (; it != this->_headers.end() && this->_statusCode != "100"; ++it) {
        ss << it->first << " " << it->second << "\r\n";
    }
    ss << "\r\n";
    std::string headers = ss.str();
   std::vector<char> payload;
    if (headers.size() > 0)
        payload.insert(payload.end(), headers.begin(), headers.end());
    if (this->_body.size() > 0)
        payload.insert(payload.end(), this->_body.begin(), this->_body.end());

    if (_logger->Env()) {
        std::cerr << _logger->Log(&Logger::LogDebug, "Create Response Payload: ");
        std::cerr << _logger->Log(&Logger::LogTrace, "Response Payload {\n", this->_toString() , "\n}");
    }
    return payload;
}

void HttpResponse::_defaultErrorPage(void) {
	std::stringstream sb;

	sb << "<html data-lt-installed=\"true\">";
	sb << "<head><title>";
	sb << this->_statusCode << " " << this->_statusMessage;
	sb << "</title></head>";
	sb << "<body>";
	sb << "<center><h1>";
	sb << this->_statusCode << " " << this->_statusMessage;
	sb << "</h1></center>";
	sb << "<hr><center>";
	sb << _server << "</center>";
	sb << "</body></html>";
    std::string page = sb.str();
    this->_body.insert(this->_body.end(), page.begin(), page.end());
    
    if (_logger->Env()) {
        std::cerr << _logger->Log(&Logger::LogDebug, "Create A Default Error Page: ");
        std::cerr << _logger->Log(&Logger::LogTrace, "Default Page {\n", this->_toString(), "\n}");
    }
}

void    HttpResponse::_createBodyByDirectory(
    std::set<std::string> dirent,
    std::string path,
    IHandler &handler) {
    std::stringstream   body;
    
    std::string actualDir = Utils::getActualDir(path);
    body << "<html data-lt-installed=\"true\">" << "\r\n";
    body << "<head>\r\n";
    body << "\t<title>Index of " << actualDir << "</title>\r\n";
    body << "</head>\r\n";
    body << "<body>\r\n";
    body << "\t<h1>index of " << actualDir << "</h1>\r\n";
    body << "\t<hr>\r\n";
    body << "\t<pre>\r\n";
    std::set<std::string>::iterator it = dirent.begin();
    for (; it != dirent.end(); ++it) {
        std::string filePath = Utils::SanitizePath(path, *it);
        if (handler.IsAllowToGetFile(filePath)) {
            if (handler.FileIsDirectory(filePath) && *it != ".." && *it != ".")
                body << "\t\t<a href=\"" << *it << "/\">" << *it << "/</a>\r\n";
            else
                body << "\t\t<a href=\"" << *it << "\">" << *it << "</a>\r\n";
            body << "\t\t" << Utils::getLastModifiedOfFile(filePath) << " " << Utils::getFileSize(filePath) << "\r\n";
        }
    }
    body << "\t</prev>\r\n";
    body << "\t<hr>\r\n";
    body << "</body>\r\n";
    body << "</html>";
    std::string page = body.str();
    this->_body.insert(this->_body.end(), page.begin(), page.end());

    if (_logger->Env()) {
        std::cerr << _logger->Log(&Logger::LogDebug, "Create A Directory Listing: ");
        std::cerr << _logger->Log(&Logger::LogTrace, "Directory Listing {\n", this->_toString(), "\n}");
    }
}

// Geters
std::string HttpResponse::GetTextContent(std::string extension) {
    return _mapTextContent[extension];
}

std::string HttpResponse::GetHttpVersion(void) const {
    return this->_HTTPVersion;
}

std::string HttpResponse::GetStatusCode(void) const {
    return this->_statusCode;
}

std::string HttpResponse::GetStatusMessage(void) const {
    return this->_statusMessage;
}

std::pair<std::string, std::string> HttpResponse::GetHeader(std::string key) {
    std::map<std::string, std::string>::iterator it = this->_headers.find(key);
    if (it != this->_headers.end()) {
        return std::make_pair(it->first, it->second);
    }
    return std::make_pair("", "");
}

std::map<std::string, std::string> HttpResponse::GetHeaders(void) const {
    return this->_headers;
}

std::string HttpResponse::GetBody(void) const {
    return std::string(this->_body.begin(), this->_body.end());
}

ssize_t HttpResponse::GetBodySize(void) const {
    return _body.size();
}

// Seters
void    HttpResponse::SetStatusCode(HttpStatusCode::Code statusCode) {
    std::stringstream code;

    code << statusCode;
    this->_statusCode = code.str();
    this->_statusMessage = _mapStatusCode[statusCode];
}

void    HttpResponse::SetHeader(std::string key, std::string value) {
    this->_headers[key] = value;
}

void    HttpResponse::SetBody(std::string body) {
    this->_body.insert(this->_body.end(), body.begin(), body.end());
}

void    HttpResponse::SetBody(std::vector<char> body) {
    this->_body = body;
}
