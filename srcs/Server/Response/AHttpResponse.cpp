# include "AHttpResponse.hpp"

std::map<std::string, std::string> AHttpResponse::_mapStatusCode;
std::map<std::string, std::string> AHttpResponse::_mapTextContent;
std::set<std::string> AHttpResponse::_CGIExtensions;

std::string AHttpResponse::CreateResponse(void) {
    std::stringstream response;

    response << this->_HTTPVersion << " " << this->_statusCode << " " << this->_statusMessage << "\r\n";
    std::map<std::string, std::string>::iterator it = this->_headers.begin();
    for (; it != this->_headers.end(); ++it) {
        response << it->first << " " << it->second << "\r\n";
    }
    response << "\r\n" << this->_body << "\r\n";
    return response.str();
}

std::string AHttpResponse::_defaultErrorPage(void) {
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
	sb << "RavyServer";
	sb << "/" << "1.27.0" << "</center>";
	sb << "</body></html>";
	return sb.str();
}

// Geters
std::string AHttpResponse::GetStatusName(std::string statusCode) {
    return _mapStatusCode[statusCode];
}

std::string AHttpResponse::GetTextContent(std::string extension) {
    return _mapTextContent[extension];
}

// Base Methods

void    AHttpResponse::SetDefaultAHTTPResponse(void) {
    static  AHttpResponse base = AHttpResponse();
}

AHttpResponse::AHttpResponse(void) {
    _setMapStatusCode();
    _setMapTextContent();
    _setCGIExtensions();
};

AHttpResponse::AHttpResponse(std::string statusCode, std::string extension, std::string body) :
    _HTTPVersion("HTTP/1.1"),
    _statusCode(statusCode),
    _statusMessage(this->GetStatusName(statusCode)),
    _body(body)
{
    this->_headers["Content-Type:"] = this->GetTextContent(extension);

    std::stringstream size;
    size << this->_body.size() + 2;
    this->_headers["Content-Length:"] = std::string(size.str());
    this->_headers["Date:"] = Utils::getCurrentTimeInGMT();
}

AHttpResponse::AHttpResponse(std::string statusCode, std::string extension) :
    _HTTPVersion("HTTP/1.1"),
    _statusCode(statusCode),
    _statusMessage(this->GetStatusName(statusCode))
{
    this->_headers["Content-Type:"] = this->GetTextContent(extension);
    this->_body = this->_defaultErrorPage();

    std::stringstream size;
    size << this->_body.size() + 2;
    this->_headers["Content-Length:"] = std::string(size.str());
    this->_headers["Date:"] = Utils::getCurrentTimeInGMT();
}


AHttpResponse::~AHttpResponse(void) {

}
