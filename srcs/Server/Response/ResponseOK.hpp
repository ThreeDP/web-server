#ifndef __RESPONSE_OK_HPP__
# define __RESPONSE_OK_HPP__

# include "AHttpResponse.hpp"

class ResponseOK : public AHttpResponse {
	private:
		ResponseOK(void) : AHttpResponse("200", "text/html", "<html></html>") {}

	public:
		ResponseOK(std::string extension, std::string body);
		ResponseOK(std::string extension);
		~ResponseOK(void);
};

#endif