#ifndef __RESPONSE_OK_HPP__
# define __RESPONSE_OK_HPP__

# include "AHttpResponse.hpp"

class Response200OK : public AHttpResponse {
	private:
		Response200OK(void) : AHttpResponse("200", "text/html", "<html></html>") {}

	public:
		Response200OK(std::string extension, std::string body);
		Response200OK(std::string extension);
		~Response200OK(void);
};

#endif