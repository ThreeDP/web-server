#ifndef __PARSER_HPP__
# define __PARSER_HPP__

# include "Http.hpp"
# include "ParserParams.hpp"
# include "BuilderServer.hpp"
# include "Logger.hpp"

class Parser {
	private:
		IHandler		*_handler;
		ILogger			*_logger;
		IBuilderServer	*_builder;

		bool		_serverOpen;
		bool		_routeOpen;
		int			_line;


	public:
		Parser(ILogger *logger, IHandler *handler, IBuilderServer *builder);
		~Parser(void);

		int		ConfigHttp(Http &http, std::string fileName);
		void    InsertService(Http &http, EnumParams::Param p, std::vector<std::string> params);
		bool	SetParamIntoRoute(Http &http, EnumParams::Param p, std::vector<std::string> &params);
		bool    SetParamIntoServer(Http &http, EnumParams::Param p, std::vector<std::string> &params);
};

#endif