#ifndef __PARSER_HPP__
# define __PARSER_HPP__

# include "Http.hpp"
# include "ParserParams.hpp"
# include "BuilderServer.hpp"

class Parser {
	private:
		IHandler		*_handler;
		IBuilderServer	*_builder;

		bool		_serverOpen;
		bool		_routeOpen;


	public:
		Parser(IHandler *handler, IBuilderServer *builder) {
			_handler = handler;
			_builder = builder;
			_serverOpen = false;
			_routeOpen = false;
		}

		int	ConfigHttp(Http &http, std::string fileName) {
			std::pair<bool, std::string> line;
			try {
				std::ifstream	*file = _handler->OpenFile(fileName);
				if (file == NULL)
					throw std::runtime_error("Error on Open Config File.");
				std::vector<std::string> pieces;
				while ((line = _handler->ReadLine(*file)).first) {
					char ch = ParserParams::SanitizeString(line.second);
					pieces.clear();
					EnumParams::Param p;
					if (ch == '\0')
						continue;
					else if (ch == '}') {
						if (this->_routeOpen) {
							this->_routeOpen = false;
							this->_builder->WithRoute(
								this->_builder->GetBuilderRoute()->GetResult()
							);
						}
						else if (this->_serverOpen) {
							this->_serverOpen = false;
							http.SetServer(this->_builder->GetResult())
						} else {
							throw std::invalid_argument("Syntax Error.");
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
					}

				}
			} catch (std::exception &e) {
				std::cerr << e.what();
				return 1;
			}
			return 0;
		}

		void	SetParamsIntoService(Http &http, EnumParams::Param p, std::vector<std::string> params) {
			if (this->_serverOpen) {
				std::pair<std::set<HttpStatusCode::Code>, std::string> code;
				if (this->_routeOpen) {
					switch (p)
					{
						case EnumParams::_SERVER:
							throw std::invalid_argument("Syntax Error.");
							break;
						case EnumParams::_ROUTE:
							throw std::invalid_argument("Syntax Error.");
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
						default:
							throw std::invalid_argument("Syntax Error.");
							break;
					}
				}
				switch (p)
				{
					case EnumParams::_SERVER:
						throw std::invalid_argument("Syntax Error.");
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
					case EnumParams::_AUTOINDEX:
						_builder->WithAutoIndex(ParserParams::GetAutoIndexParam(params));
						break;
					case EnumParams::_ROUTE:
						_builder->GetBuilderRoute()->SetupRoute(ParserParams::GetStringParam(params));
						break;	
					default:
						throw std::invalid_argument("Syntax Error.");
						break;
				}
			} else if (p == EnumParams::_SERVER) {
				_builder->SetupServer();
				this->_serverOpen = true;
			}
			throw std::invalid_argument("Syntax Error.");
		}
};

#endif