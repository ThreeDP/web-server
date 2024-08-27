#ifndef __PARSER_HPP__
# define __PARSER_HPP__

# include "Http.hpp"
# include "BuilderServer.hpp"
# include "Logger.hpp"
# include <algorithm>

namespace EnumParams {

	enum Param {
		_UNW,
		_ROOT,
		_REWRITE,
		_SERVER_NAME,
		_SERVER,
		_LISTEN,
		_BODY_LIMIT,
		_ERROR_PAGE,
		_ALLOW_METHODS,
		_AUTOINDEX,
		_INDEX,
		_ROUTE
	};
}

class Parser {
	private:
		IHandler		*_handler;
		ILogger			*_logger;
		IBuilderServer	*_builder;

		bool			_serverOpen;
		bool			_routeOpen;
		int				_line;
		std::ifstream	*_file;


	public:
		Parser(ILogger *logger, IHandler *handler, IBuilderServer *builder);
		~Parser(void);

		int		ConfigHttp(Http &http, std::string fileName);
		void    InsertService(EnumParams::Param p, std::vector<std::string> params);
		bool	SetParamIntoRoute(EnumParams::Param p, std::vector<std::string> &params);
		bool    SetParamIntoServer(EnumParams::Param p, std::vector<std::string> &params);

		EnumParams::Param			IdentifiesParameter(std::string input);
		std::vector<std::string> 	BreakLineIntoPieces(std::string input, char c);
		std::vector<std::string> 	GetVectorParams(std::vector<std::string> vector, std::string end);
		std::set<std::string> 		GetSetParams(std::vector<std::string> vector, std::string end);
		std::pair<std::string, std::string>
									GetPairParams(std::vector<std::string> vector, std::string end);
		std::pair<std::set<HttpStatusCode::Code>, std::string>
									GetPairCodeParams(std::vector<std::string> vector, std::string end);
		std::string					GetStringParam(std::vector<std::string> vector, std::string end);
		int 						GetBodyLimitParam(std::vector<std::string> vector, std::string end);
		bool						GetAutoIndexParam(std::vector<std::string> vector, std::string end);
		char 						SanitizeString(std::string str);
		char						ExceptionValidation(std::string title, std::string params, std::string set);
};

#endif