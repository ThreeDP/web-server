# include "HttpResponse.hpp"

std::map<HttpStatusCode::Code, std::string> HttpResponse::_mapStatusCode;
std::map<std::string, std::string> HttpResponse::_mapTextContent;
std::set<std::string> HttpResponse::_CGIExtensions;

std::vector<char> HttpResponse::CreateResponse(void) {
    std::stringstream ss;
    std::stringstream bodySize;

    bodySize << this->_body.size() + 2;
    this->_headers["Content-Length:"] = bodySize.str();
    this->_headers["Date:"] = Utils::getCurrentTimeInGMT();
    ss << this->_HTTPVersion << " " << this->_statusCode << " " << this->_statusMessage << "\r\n";
    std::map<std::string, std::string>::iterator it = this->_headers.begin();
    for (; it != this->_headers.end(); ++it) {
        ss << it->first << " " << it->second << "\r\n";
    }
    ss << "\r\n";

    std::string headers = ss.str();
    std::vector<char> payload(headers.begin(), headers.end());
    payload.insert(payload.end(), this->_body.begin(), this->_body.end());
    std::string endend = "\r\n";
    payload.insert(payload.end(), endend.begin(), endend.end());
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
}

// Geters
std::string HttpResponse::GetStatusName(HttpStatusCode::Code statusCode) {
    return _mapStatusCode[statusCode];
}

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
    std::string str(body.begin(), body.end());
    std::cout << str << std::endl;
    this->_body = body;
}

// Base Methods
HttpResponse::HttpResponse(int num) {
    _setMapStatusCode();
    _setMapTextContent();
    _setCGIExtensions();
};

HttpResponse::HttpResponse(void) :
    _HTTPVersion("HTTP/1.1")
{
    this->_headers["Content-Type:"] = this->_mapTextContent["text"];
    this->_headers["Server:"] = this->_server;
}