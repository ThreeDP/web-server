#ifndef __HTTP_REQUEST_HPP__
# define __HTTP_REQUEST_HPP__

# include <map>
# include <iostream>
# include <sstream>

class HttpRequest {
    protected:
        std::string                         _method;
        std::string                         _path;
        std::string                         _HTTPVersion;
        std::string                         _body;

    public:
        std::map<std::string, std::string>  _payload;
        
        void                ParserRequest(std::string request);

        // Base Methods
        HttpRequest();
        ~HttpRequest();

        // Geters
        std::string         GetMethod(void) const;
        std::string         GetPath(void) const;
        std::string         GetHTTPVersion(void) const;
        std::string         GetBody(void) const;
};

#endif