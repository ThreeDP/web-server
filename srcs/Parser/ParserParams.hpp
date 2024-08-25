#ifndef __PARSER_PARAMS_HPP__
# define __PARSER_PARAMS_HPP__

#include "EHttpStatusCode.hpp"
#include <algorithm>

namespace EnumParams {

	enum Param {
		_NOPARAM,
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
			map["location"] = EnumParams::_ROUTE;

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

		static std::vector<std::string> GetVectorParams(std::vector<std::string> vector) {
			if (vector.empty() || vector.size() <= 2)
				throw std::invalid_argument("8 Syntax Error.");
			std::vector<std::string>::iterator start = vector.begin() + 1;
			std::vector<std::string>::iterator end = vector.end() - 1;
			return std::vector<std::string>(start, end);
		}

		static std::set<std::string> GetSetParams(std::vector<std::string> vector) {
			if (vector.empty() || vector.size() <= 2)
				throw std::invalid_argument("9 Syntax Error.");
			std::vector<std::string>::iterator it = vector.begin() + 1;
			std::set<std::string> newSet;
			for ( ; it != vector.end() - 1; ++it) {
				newSet.insert(*it);
			}
			return newSet;
		}

		static std::pair<std::string, std::string> GetPairParams(std::vector<std::string> vector) {
			int size;
			if (!vector.empty() && (size = vector.size() - 2) == 2) {
				return std::pair<std::string, std::string>(vector[1], vector[2]);
			}
			std::stringstream ss;
			ss << "Wrong number of params for {" << "command" << "}." << std::endl; 
			throw std::invalid_argument(ss.str());
		}

		static std::pair<std::set<HttpStatusCode::Code>, std::string> GetPairCodeParams(std::vector<std::string> vector) {
			int size = vector.size() - 2;
			std::stringstream ss;
			ss << "Wrong number of params for {" << vector[0] << "}." << std::endl; 
			if (size < 2)
				throw std::invalid_argument(ss.str());
			std::vector<std::string>::iterator it = vector.begin() + 1;
			std::set<HttpStatusCode::Code> codes;
			for (; it != vector.end() - 2; ++it) {
				    int code;
					std::stringstream str(*it);
					str >> code;
					if (str.fail()) {
						throw std::invalid_argument("Invalid value on code.");
					}
					codes.insert((HttpStatusCode::Code)code);
			}
			return std::pair<std::set<HttpStatusCode::Code>, std::string>(codes, *it);
		}

		static std::string GetStringParam(std::vector<std::string> vector) {
			int size = vector.size() - 2;
			std::stringstream ss;
			ss << "Wrong number of params for {" << vector[0] << "}." << std::endl; 
			if (size != 1)
				throw std::invalid_argument(ss.str());

			return std::string(vector[1]);
		}

		static int GetBodyLimitParam(std::vector<std::string> vector) {
			int size = vector.size() - 2;
			std::stringstream ss;
			ss << "Wrong number of params for {" << vector[0] << "}." << std::endl; 
			if (size != 1)
				throw std::invalid_argument(ss.str());
			return 2048;
		}

		static bool GetAutoIndexParam(std::vector<std::string> vector) {
			int size = vector.size() - 2;
			std::stringstream ss;
			ss << "Wrong number of params for {" << vector[0] << "}." << std::endl; 
			if (size != 1)
				throw std::invalid_argument(ss.str());
			if (vector[1] == "on")
				return true;
			else if (vector[1] == "off")
				return false;
			throw std::invalid_argument("Wrong Param on autoindex options:(on, off).");
		}

		static char SanitizeString(std::string str) {
			// if (str.) {
			// 	return 'T';
			// }
			int semicolon = std::count(str.begin(), str.end(), ';');
			int open_curly = std::count(str.begin(), str.end(), '{');
			int close_curly = std::count(str.begin(), str.end(), '}');
			if ((semicolon + open_curly + close_curly) > 1) {
				throw std::invalid_argument("10 Syntax Error.");
			} else if ((semicolon + open_curly + close_curly) == 0) {
				int size = 0;
				for (std::string::iterator it = str.begin(); it != str.end(); ++it) {
					size++;
				}
				return (size == str.size()) ? 'T'
					: throw std::invalid_argument("11 Syntax Error.");
			}
			if (semicolon)
				return ';';
			else if (open_curly)
				return '{';
			else if (close_curly)
				return '}';
			throw std::invalid_argument("12 Syntax Error.");
		}
};

#endif