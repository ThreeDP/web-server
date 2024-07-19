#ifndef __RESPONSE_201_CREATED_HPP__
# define __RESPONSE_201_CREATED_HPP__

# include "AHttpResponse.hpp"

class Response201Created : public AHttpResponse {
    private:
        Response201Created(void) : AHttpResponse("201", "text/html", "<html></html>") {}
    public:
        Response201Created(std::string extension, std::string location);
        Response201Created(std::string extension, std::string location, std::string body);
        ~Response201Created(void);
};

#endif