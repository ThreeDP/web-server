#ifndef __IHTTP_RESPONSE_HPP__
# define __IHTTP_RESPONSE_HPP__

# include "EHttpStatusCode.hpp"
# include <vector>

class IBuilderResponse;

class IHttpResponse {
    public:
        virtual void _defaultErrorPage(void) = 0;
        virtual void _createBodyByDirectory(std::vector<struct dirent*> *dirent) = 0;
        virtual void _createBodyByFileDescriptor(std::ifstream *fd) = 0;
        virtual std::string CreateResponse(void) = 0;
        virtual std::string ToString(void) = 0;

        // Geters
        virtual std::string GetStatusName(HttpStatusCode::Code statusCode) = 0;
        virtual std::string GetTextContent(std::string extension) = 0;
        virtual std::string GetBody(void) const = 0;

        // Seters
        virtual ~IHttpResponse() {};

        // Seters
        virtual void SetStatusCode(HttpStatusCode::Code statusCode) = 0;
        virtual void SetHeader(std::string key, std::string value) = 0;
        virtual void SetBody(std::string body) = 0;


        friend class IBuilderResponse;
};

#endif