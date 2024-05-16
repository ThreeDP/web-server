#ifndef __PARSER_HPP__
# define __PARSER_HPP__

# include <map>
# include <iterator>
# include <fstream>
# include <iostream>
# include <iomanip>
# include <sstream>

class Parser {
    private:
        Parser();

        std::ifstream _fd;


    public:
        Parser(std::string fileName);

        class Except: virtual public std::exception {
			protected:
			std::string error_message;
			public:
				explicit Except(const std::string& msg) : error_message(msg) {}
				virtual ~Except() throw () {}
				virtual const char* what() const throw () { return error_message.c_str();}
		};
};

#endif