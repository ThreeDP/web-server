# include "Parser.hpp"

Parser::Parser(std::string fileName) : _fd(fileName.c_str()) {
    if (!this->_fd) {
        throw (Except("Configuration File Not Found!"));
    }
}