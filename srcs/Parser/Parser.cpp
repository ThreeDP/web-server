# include "Parser.hpp"

Parser::Parser(ILogger *logger, IHandler *handler, IBuilderServer *builder) :
    _handler(handler),
    _logger(logger),
    _builder(builder),
    _routeOpen(false),
    _serverOpen(false),
    _line(0),
    _file(NULL)
{

}

Parser::~Parser(void) {
    if (_file != NULL) {
        _file->close();
        delete _file;
    }
}

int	Parser::ConfigHttp(Http &http, std::string fileName) {
    std::pair<bool, std::string> line;
    std::ifstream	*_file = _handler->OpenFile(fileName);
    if (_file == NULL)
        throw std::runtime_error(_logger->Log(
            &Logger::LogCaution,
            "Error to open config file:",
            fileName
        ));
    while ((line = _handler->ReadLine(*_file)).first) {
        this->_line++;
        EnumParams::Param p;
        std::vector<std::string> pieces;
        char ch = this->SanitizeString(line.second);

        if (ch == '}') {
            if (this->_routeOpen) {
                this->_routeOpen = false;
                this->_builder->WithRoute(
                    this->_builder->GetBuilderRoute()->GetResult()
                );
            }
            else if (this->_serverOpen) {
                this->_serverOpen = false;
                http.SetServer(this->_builder->GetResult());
            } else {
                ExceptionValidation("Syntax error on line", Utils::VectorToString(pieces), "18");
            }
            continue;
        } else if (ch == ';') {
            pieces = this->BreakLineIntoPieces(line.second, ';');
            if (pieces.size() > 1) {
                p = this->IdentifiesParameter(pieces[0]);
            }
        } else if (ch == '{') {
            pieces = this->BreakLineIntoPieces(line.second, '{');
            if (pieces.size() > 1) {
                p = this->IdentifiesParameter(pieces[0]);
            }
        } else if (ch == '\0') {
            continue;
        } else {
            ExceptionValidation("Syntax error on line", Utils::VectorToString(pieces), "17");
        }
        if (_logger->Env())
            _logger->Log(&Logger::LogTrace, "Line:", Utils::VectorToString(pieces), "\tFlag:", p);
        this->InsertService(http, p, pieces);
    }
    if (_file != NULL) {
        _file->close();
        delete _file;
    }
    return 0;
}

void    Parser::InsertService(Http &http, EnumParams::Param p, std::vector<std::string> params) {
    if (params.empty())
        ExceptionValidation("Syntax error on line", "", "16");
    if (SetParamIntoRoute(http, p, params)) {return;}
    if (SetParamIntoServer(http, p, params)) {return;}
    else if (p == EnumParams::_SERVER && params.size() == 2 && params.back() == "{") {
        this->_serverOpen = true;
        _builder->SetupServer();
    } else {
        throw std::invalid_argument(_logger->Log(
            &Logger::LogCaution,
            "Error on Config Service: Syntax Error on line [",
            _line, "] ", Utils::VectorToString(params)
        ));
    }
}

bool    Parser::SetParamIntoRoute(Http &http, EnumParams::Param p, std::vector<std::string> &params) {
    if (this->_serverOpen) {
        if (this->_routeOpen) {
            std::pair<std::set<HttpStatusCode::Code>, std::string> code;
            switch (p)
            {
                case EnumParams::_SERVER:
                    throw std::invalid_argument(
                        _logger->Log(&Logger::LogCaution,
                        "Syntax Error: on line [",
                        _line,
                        "] is not possible config a Server inside a Route"
                    ));
                    break;
                case EnumParams::_ROUTE:
                    throw std::invalid_argument(
                        _logger->Log(&Logger::LogCaution,
                        "Syntax Error: on line [",
                        _line,
                        "] is not possible config a Route inside a Route"
                    ));
                    break;	
                case EnumParams::_REWRITE:
                    _builder->GetBuilderRoute()->WithRedirectPath(this->GetStringParam(params, ";"));
                    break;
                case EnumParams::_ROOT:
                    _builder->GetBuilderRoute()->WithRootDirectory(this->GetStringParam(params, ";"));
                    break;
                case EnumParams::_ERROR_PAGE:
                    code = this->GetPairCodeParams(params, ";");
                    _builder->GetBuilderRoute()->WithErrorPage(code.first, code.second);
                    break;
                case EnumParams::_BODY_LIMIT:
                    _builder->GetBuilderRoute()->WithBodyLimit(this->GetBodyLimitParam(params, ";"));
                    break;
                case EnumParams::_ALLOW_METHODS:
                    _builder->GetBuilderRoute()->WithAllowMethods(this->GetSetParams(params, ";"));
                    break;
                case EnumParams::_AUTOINDEX:
                    _builder->GetBuilderRoute()->WithAutoIndex(this->GetAutoIndexParam(params, ";"));
                    break;
                case EnumParams::_INDEX:
                    _builder->GetBuilderRoute()->WithPageIndexes(this->GetVectorParams(params, ";"));
                    break;
                default:
                    ExceptionValidation("Syntax error on line", Utils::VectorToString(params), "15");
                    break;
            }
            return true;
        }
    }
    return false;
}

bool    Parser::SetParamIntoServer(Http &http, EnumParams::Param p, std::vector<std::string> &params) {
    if (this->_serverOpen) {
        std::pair<std::set<HttpStatusCode::Code>, std::string> code;
        switch (p)
        {
            case EnumParams::_SERVER:
                throw std::invalid_argument(
                    _logger->Log(&Logger::LogCaution,
                    "Syntax Error: on line [",
                    _line,
                    "] is not possible config a Server inside a Server"
                ));
                break;
            case EnumParams::_REWRITE:
                _builder->WithRedirectPath(this->GetPairParams(params, ";"));
                break;
            case EnumParams::_ROOT:
                _builder->WithRootDirectory(this->GetStringParam(params, ";"));
                break;
            case EnumParams::_LISTEN:
                _builder->WithPort(this->GetStringParam(params, ";"));
                break;
            case EnumParams::_ERROR_PAGE:
                code = this->GetPairCodeParams(params, ";");
                _builder->WithErrorPages(code.first, code.second);
                break;
            case EnumParams::_BODY_LIMIT:
                _builder->WithBodyLimit(this->GetBodyLimitParam(params, ";"));
                break;
            case EnumParams::_ALLOW_METHODS:
                _builder->WithAllowMethods(this->GetSetParams(params, ";"));
                break;
            case EnumParams::_INDEX:
                _builder->WithIndexes(this->GetVectorParams(params, ";"));
                break;
            case EnumParams::_AUTOINDEX:
                _builder->WithAutoIndex(this->GetAutoIndexParam(params, ";"));
                break;
            case EnumParams::_ROUTE:
                this->_routeOpen = true;
                _builder->GetBuilderRoute()->SetupRoute(this->GetStringParam(params, "{"));
                break;
            case EnumParams::_SERVER_NAME:
                _builder->WithHosts(this->GetVectorParams(params, ";"));
                break;	
            default:
                ExceptionValidation("Syntax error on line", Utils::VectorToString(params), "14");
                break;
        }
        return true;
    }
    return false;
}

EnumParams::Param Parser::IdentifiesParameter(std::string input) {
    std::map<std::string, EnumParams::Param> map;
    map["rewrite"] = EnumParams::_REWRITE;
    map["root"] = EnumParams::_ROOT;
    map["server_name"] = EnumParams::_SERVER_NAME;
    map["server"] = EnumParams::_SERVER;
    map["listen"] = EnumParams::_LISTEN;
    map["error_page"] = EnumParams::_ERROR_PAGE;
    map["client_max_body_size"] = EnumParams::_BODY_LIMIT;
    map["allow_methods"] = EnumParams::_ALLOW_METHODS;
    map["autoindex"] = EnumParams::_AUTOINDEX;
    map["index"] = EnumParams::_INDEX;
    map["location"] = EnumParams::_ROUTE;

    std::map<std::string, EnumParams::Param>::iterator it = map.find(input);
    if (it != map.end())
        return it->second;
    ExceptionValidation("Invalid Param on line", input, "13");
}

std::vector<std::string> Parser::BreakLineIntoPieces(std::string input, char c) {
    int hasFinish = input.find_last_of(c);

    std::string ch(1, c);
    if (hasFinish != input.size() - 1)
        throw std::invalid_argument(
            _logger->Log(
                &Logger::LogCaution,
                "Syntax error on line [",
                _line,
                "] not found -",
                ch
        ));
    std::stringstream ss(input.substr(0, hasFinish));
    std::string word;
    std::vector<std::string> pieces;
    while (ss >> word) {
        pieces.push_back(word);
    }
    pieces.push_back(ch);
    return pieces;
}

char    Parser::ExceptionValidation(std::string title, std::string params, std::string set) {
    throw std::invalid_argument(_logger->Log(
        &Logger::LogCaution,
        set,
        title,
        "[",
        _line,
        "] - (",
        params,
        ")."
    ));
    return ('\0');
}

std::vector<std::string> Parser::GetVectorParams(std::vector<std::string> vector, std::string end) {
    if (vector.size() <= 2 || vector.back() != end)
        ExceptionValidation("Syntax error on line", Utils::VectorToString(vector), "0");
    std::vector<std::string>::iterator start = vector.begin() + 1;
    std::vector<std::string>::iterator finish = vector.end() - 1;
    return std::vector<std::string>(start, finish);
}

std::set<std::string> Parser::GetSetParams(std::vector<std::string> vector, std::string end) {
    if (vector.size() <= 2 || vector.back() != end)
        ExceptionValidation("Syntax error on line", Utils::VectorToString(vector), "1");
    std::vector<std::string>::iterator it = vector.begin() + 1;
    std::set<std::string> newSet;
    for ( ; it != vector.end() - 1; ++it) {
        newSet.insert(*it);
    }
    return newSet;
}

std::pair<std::string, std::string> Parser::GetPairParams(std::vector<std::string> vector, std::string end) {
    if ((vector.size() - 2) == 2 && vector.back() == end)
        return std::pair<std::string, std::string>(vector[1], vector[2]);
    ExceptionValidation("Syntax error on line", Utils::VectorToString(vector), "2");

}

std::pair<std::set<HttpStatusCode::Code>, std::string> Parser::GetPairCodeParams(std::vector<std::string> vector, std::string end) {
    if (vector.size() <= 2 || vector.back() != end)
        ExceptionValidation("Syntax error on line", Utils::VectorToString(vector), "3");
    std::vector<std::string>::iterator it = vector.begin() + 1;
    std::set<HttpStatusCode::Code> codes;
    for (; it != vector.end() - 2; ++it) {
            int code;
            std::stringstream str(*it);
            str >> code;
            if (str.fail()) {
                ExceptionValidation("Invalid param error on line", Utils::VectorToString(vector), "4");
            }
            codes.insert((HttpStatusCode::Code)code);
    }
    return std::pair<std::set<HttpStatusCode::Code>, std::string>(codes, *it);
}

std::string Parser::GetStringParam(std::vector<std::string> vector, std::string end) { 
    if ((vector.size() - 2) != 1 || vector.back() != end)
        ExceptionValidation("Syntax error on line", Utils::VectorToString(vector), "5");
    return std::string(vector[1]);
}

int Parser::GetBodyLimitParam(std::vector<std::string> vector, std::string end) {
    if ((vector.size() - 2) != 1 || vector.back() != end)
        return (-1, ExceptionValidation("Syntax error on line", Utils::VectorToString(vector), "6"));
    size_t bodyLimit = 0;
    std::stringstream ss(vector[1]);
    ss >> bodyLimit;
    if (ss.fail())
        ExceptionValidation("Invalid param error on line", Utils::VectorToString(vector), "7");
    return bodyLimit;
}

bool Parser::GetAutoIndexParam(std::vector<std::string> vector, std::string end) {
    if ((vector.size() - 2) != 1 || vector.back() != end)
        ExceptionValidation("Syntax error on line", Utils::VectorToString(vector), "8");
    if (vector[1] == "on")
        return true;
    else if (vector[1] == "off")
        return false;
    ExceptionValidation("Invalid param on line", Utils::VectorToString(vector), "9");
}

char Parser::SanitizeString(std::string str) {
    int semicolon = std::count(str.begin(), str.end(), ';');
    int open_curly = std::count(str.begin(), str.end(), '{');
    int close_curly = std::count(str.begin(), str.end(), '}');
    if ((semicolon + open_curly + close_curly) > 1) {
        return ExceptionValidation("Syntax error on line", str, "10");
    } else if ((semicolon + open_curly + close_curly) == 0) {
        int size = 0;
        for (std::string::iterator it = str.begin(); it != str.end(); ++it) {
            if (std::isspace(*it))
                size++;
        }
        return (size == str.size()) ? '\0' : ExceptionValidation("Syntax error on line", str, "11");

    }
    if (semicolon)
        return ';';
    else if (open_curly)
        return '{';
    else if (close_curly)
        return '}';
    return ExceptionValidation("Syntax error on line", str, "12");
}