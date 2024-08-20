#ifndef __PARSER_PARAMS_HPP__
# define __PARSER_PARAMS_HPP__

namespace EnumParams {

	enum Param {
		_ROOT,
		_REWRITE,
		_SERVER_NAME,
		_SERVER,
		_LISTEN,
		_BODY_LIMIT,
		_ERROR_PAGE,
		_ALLOW_METHODS,
		_AUTOINDEX,
		_INDEX
	};
}

class ParserParams {

	public:
		static EnumParams::Param IdentifiesParameter(std::string input) {
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

			std::map<std::string, EnumParams::Param>::iterator it = map.find(input);
			if (it != map.end())
				return it->second;
			throw std::invalid_argument("Parameter does not exist.");
		}

		static std::vector<std::string> BreakLineIntoPieces(std::string input, char c) {
			int hasFinish = input.find_last_of(c);
			std::stringstream errMsg;

			errMsg << "Param does not end with (" << c << ")" << std::endl;
			if (hasFinish != input.size() - 1)
				throw std::invalid_argument(errMsg.str().c_str());
			std::stringstream ss(input.substr(0, hasFinish));
			std::string word;
			std::vector<std::string> pieces;
			while (ss >> word) {
				pieces.push_back(word);
			}
			pieces.push_back(std::string(1, c));
			return pieces;
		}
};

#endif