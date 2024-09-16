# include "BuilderResponse.hpp"

// Contructors
BuilderResponse::BuilderResponse(ILogger *logger, IHandler *handler) : 
    _logger(logger),
    _handler(handler)
{
    std::cerr << _logger->Log(&Logger::LogDebug, "Created BuilderResponse Class.");
    _response = NULL;
}

BuilderResponse::~BuilderResponse(void) {
    std::cerr << _logger->Log(&Logger::LogDebug, "Deleted BuilderResponse Class.");
    if (_response != NULL) {
        delete _response;
        _response = NULL;
    }
}

// Setup Response
IBuilderResponse &BuilderResponse::SetupResponse(void) {
    if (_response != NULL) {
        delete _response;
        _response = NULL;
    }
    _response = new HttpResponse(_logger);
    return *this;
}

IBuilderResponse &BuilderResponse::WithStatusCode(HttpStatusCode::Code statusCode) {
    _response->SetStatusCode(statusCode);
    return *this; 
}

IBuilderResponse &BuilderResponse::WithHeader(std::pair<std::string, std::string> pair) {
    _response->SetHeader(pair.first, pair.second);
    return *this;
}

IBuilderResponse &BuilderResponse::WithDirectoryFile(DIR *directory, std::string path) {
    if (directory != NULL && _response->GetBodySize() == 0) {
        std::set<std::string> directories = _handler->ReadDirectory(directory);
        _response->_createBodyByDirectory(directories, path, *_handler);
    }
    closedir(directory);
    directory = NULL;
    return *this;
}


IBuilderResponse &BuilderResponse::WithBody(std::vector<char> body) {
    if (_response->GetBody() == "") {
        _response->SetBody(body);
        
        if (_logger->Env()) {
            std::cerr << _logger->Log(&Logger::LogDebug, "Create A Body CGI: ");
            std::cerr << _logger->Log(&Logger::LogTrace, "Payload CGI {\n", _response->_toString(), "\n}");
        }
    }
    return *this;
}

IBuilderResponse &BuilderResponse::WithFileDescriptor(std::ifstream *fd) {
    if (fd != NULL && _response->GetBodySize() == 0) {
        _response->SetBody(_handler->ReadRegularFile(fd));
        if (_response->GetBodySize() == 0)
            _response->SetStatusCode(HttpStatusCode::_NO_CONTENT);
        if (_logger->Env()) {
            std::cerr << _logger->Log(&Logger::LogDebug, "Create A Body File Descriptor: ");
            std::cerr << _logger->Log(&Logger::LogTrace, "Payload File Descriptor {\n", _response->_toString(), "\n}");
        }
    }
    fd->close();
    delete fd;
    return *this;
}

IBuilderResponse &BuilderResponse::WithDefaultPage(void) {
    if (_response->GetBodySize() == 0) {
        _response->_defaultErrorPage();
    }
    return *this;
}

IBuilderResponse &BuilderResponse::WithContentType(std::string extension) {
    _response->SetHeader("Content-Type:", _response->GetTextContent(extension));
    return *this; 
};

IBuilderResponse &BuilderResponse::WithLastModified(std::string path) {
    _response->SetHeader("Content-Type:", _response->GetTextContent(Utils::getLastModifiedOfFile(path)));
    return *this; 
};

IBuilderResponse &BuilderResponse::WithLocation(std::string location) {
    _response->SetHeader("Location:", location);
    return *this;
};

IHttpResponse *BuilderResponse::GetResult(void) {
    IHttpResponse *res = _response;
    _response = NULL;
    return res;
}

bool BuilderResponse::CompareResponses(IHttpResponse &left, IHttpResponse &right) {
    bool res = left.GetHttpVersion() == right.GetHttpVersion()
        && left.GetStatusCode() == right.GetStatusCode()
        && left.GetStatusMessage() == right.GetStatusMessage();
    std::map<std::string, std::string> leftHeaders = left.GetHeaders();
    std::map<std::string, std::string> rightHeaders = right.GetHeaders();
    std::map<std::string, std::string>::iterator it = leftHeaders.begin();
    for ( ; it != leftHeaders.end(); ++it) {
        res = res && it->second == rightHeaders[it->first];
    }
    std::cout << "Left: " << left.GetBody() << "\n\n";
    std::cout << "Right: " << right.GetBody() << "\n\n";
    return res && left.GetBody() == right.GetBody();
}

IBuilderResponse &BuilderResponse::ParserResponse(std::vector<char> response) {
       std::string str(response.begin(), response.end());
    size_t firstHeaderEnd = str.find("\r\n\r\n");
    if (firstHeaderEnd == std::string::npos) {
        firstHeaderEnd = str.size();
    } else {
        firstHeaderEnd += 4;
    }

    ssize_t firstSizeLine = 0;
    std::stringstream srequest(str.substr(firstSizeLine, firstHeaderEnd));
    std::string line;

    if (std::getline(srequest, line, '\n')) {
        firstSizeLine = line.size();
        std::stringstream requestLine(line);
        std::string label;
        std::getline(requestLine, label, ' ');
        std::getline(requestLine, label, ' ');
        HttpStatusCode::Code status = (label == "201" ? HttpStatusCode::_CREATED : label == "500" ? HttpStatusCode::_BAD_GATEWAY : HttpStatusCode::_OK);
        WithStatusCode(status);
        std::getline(requestLine, label, '\r');
    }

    std::stringstream headers(str.substr(firstSizeLine, firstHeaderEnd));
    while (std::getline(headers, line, '\n')) {
        std::string key;
        std::string value;
        std::stringstream headerLine(line);
        if (std::getline(std::getline(headerLine, key, ':'), value)) {
            value.erase(0, value.find_first_not_of(" \r"));
            WithHeader(make_pair(key + ":", value));
        }
    }
    std::vector<char> body;
    if (response.size() > firstHeaderEnd) {
        body.assign(response.begin() + firstHeaderEnd, response.end());
        WithBody(body);
    }
    return *this;
}