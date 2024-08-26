# include "Parser.hpp"

Parser::Parser(ILogger *logger, IHandler *handler, IBuilderServer *builder) :
    _handler(handler),
    _logger(logger),
    _builder(builder),
    _routeOpen(false),
    _serverOpen(false),
    _line(0)
{

}

Parser::~Parser(void) {

}

int	Parser::ConfigHttp(Http &http, std::string fileName) {
    std::pair<bool, std::string> line;
    std::ifstream	*file = _handler->OpenFile(fileName);
    if (file == NULL)
        throw std::runtime_error(_logger->Log(
            &Logger::LogCaution,
            "Error to open config file:",
            fileName
        ));
    while ((line = _handler->ReadLine(*file)).first) {
        this->_line++;
        EnumParams::Param p;
        std::vector<std::string> pieces;
        char ch = ParserParams::SanitizeString(line.second);

        std::cout << std::string(1, ch) << " ";
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
                throw std::invalid_argument("1 Syntax Error.");
            }
            continue;
        } else if (ch == ';') {
            pieces = ParserParams::BreakLineIntoPieces(line.second, ';');
            if (pieces.size() > 1) {
                p = ParserParams::IdentifiesParameter(pieces[0]);
            }
        } else if (ch == '{') {
            pieces = ParserParams::BreakLineIntoPieces(line.second, '{');
            if (pieces.size() > 1) {
                p = ParserParams::IdentifiesParameter(pieces[0]);
            }
        } else if (ch == '\0') {
            continue;
        } else {
            std::stringstream ss;
            std::vector<std::string>::iterator it = pieces.begin();
            for ( ; it != pieces.end(); ++it) {
                ss << *it << " ";
            }
            throw std::invalid_argument(_logger->Log(
                &Logger::LogCaution,
                "Syntax Error on line: [",
                this->_line,
                "] ",
                ss.str()
            ));
        }
        this->InsertService(http, p, pieces);
    }
    return 0;
}

void    Parser::InsertService(Http &http, EnumParams::Param p, std::vector<std::string> params) {
    std::cout << params[0] << std::endl;
    if (SetParamIntoRoute(http, p, params)) {return;}
    if (SetParamIntoServer(http, p, params)) {return;}
    else if (p == EnumParams::_SERVER) {
        this->_serverOpen = true;
        _builder->SetupServer();
    } else {
        std::stringstream ss;
        std::vector<std::string>::iterator it = params.begin();
        for (; it != params.end(); ++it) { ss << *it << " "; }
        throw std::invalid_argument(_logger->Log(
            &Logger::LogCaution,
            "Error on Config Service: Syntax Error on line [",
            _line, "] ", ss.str()));
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
                    _builder->GetBuilderRoute()->WithRedirectPath(ParserParams::GetStringParam(params));
                    break;
                case EnumParams::_ROOT:
                    _builder->GetBuilderRoute()->WithRootDirectory(ParserParams::GetStringParam(params));
                    break;
                case EnumParams::_ERROR_PAGE:
                    code = ParserParams::GetPairCodeParams(params);
                    _builder->GetBuilderRoute()->WithErrorPage(code.first, code.second);
                    break;
                case EnumParams::_BODY_LIMIT:
                    _builder->GetBuilderRoute()->WithBodyLimit(ParserParams::GetBodyLimitParam(params));
                    break;
                case EnumParams::_ALLOW_METHODS:
                    _builder->GetBuilderRoute()->WithAllowMethods(ParserParams::GetSetParams(params));
                    break;
                case EnumParams::_AUTOINDEX:
                    _builder->GetBuilderRoute()->WithAutoIndex(ParserParams::GetAutoIndexParam(params));
                    break;
                case EnumParams::_INDEX:
                    _builder->GetBuilderRoute()->WithPageIndexes(ParserParams::GetVectorParams(params));
                    break;
                default:
                    throw std::invalid_argument("4 Syntax Error.");
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
                _builder->WithRedirectPath(ParserParams::GetPairParams(params));
                break;
            case EnumParams::_ROOT:
                _builder->WithRootDirectory(ParserParams::GetStringParam(params));
                break;
            case EnumParams::_LISTEN:
                _builder->WithPort(ParserParams::GetStringParam(params));
                break;
            case EnumParams::_ERROR_PAGE:
                code = ParserParams::GetPairCodeParams(params);
                _builder->WithErrorPages(code.first, code.second);
                break;
            case EnumParams::_BODY_LIMIT:
                _builder->WithBodyLimit(ParserParams::GetBodyLimitParam(params));
                break;
            case EnumParams::_ALLOW_METHODS:
                _builder->WithAllowMethods(ParserParams::GetSetParams(params));
                break;
            case EnumParams::_INDEX:
                _builder->WithIndexes(ParserParams::GetVectorParams(params));
                break;
            case EnumParams::_AUTOINDEX:
                _builder->WithAutoIndex(ParserParams::GetAutoIndexParam(params));
                break;
            case EnumParams::_ROUTE:
                this->_routeOpen = true;
                _builder->GetBuilderRoute()->SetupRoute(ParserParams::GetStringParam(params));
                break;
            case EnumParams::_SERVER_NAME:
                _builder->WithHosts(ParserParams::GetVectorParams(params));
                break;	
            default:
                throw std::invalid_argument("6 Syntax Error.");
                break;
        }
        return true;
    }
    return false;
}