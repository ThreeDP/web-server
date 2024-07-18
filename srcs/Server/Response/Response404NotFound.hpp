#ifndef __RESPONSE_404_NOT_FOUND__
# define __RESPONSE_404_NOT_FOUND__

# include "AHttpResponse.hpp"

class Response404NotFound : public AHttpResponse {
	Response404NotFound(void) : AHttpResponse("404", "text/html", "<html></html>") {}

	public:
		Response404NotFound(std::string extension, std::string body);
		Response404NotFound(std::string extension);
		~Response404NotFound(void);
};

#endif