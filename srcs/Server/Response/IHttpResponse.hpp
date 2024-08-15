#ifndef __IHTTP_RESPONSE_HPP__
# define __IHTTP_RESPONSE_HPP__

# include "EHttpStatusCode.hpp"
# include "IHandler.hpp"
# include <vector>

class IBuilderResponse;

class IHttpResponse {
    private:
        virtual void _defaultErrorPage(void) = 0;
        virtual void _createBodyByDirectory(std::set<std::string> dirent, std::string path, IHandler &handler) = 0;
        virtual void _createBodyByFileDescriptor(std::ifstream *fd) = 0;
        virtual std::string             GetHttpVersion(void) const = 0;
        virtual std::string             GetStatusCode(void) const = 0;
        virtual std::string             GetStatusMessage(void) const = 0;
        virtual std::pair<std::string, std::string> GetHeader(std::string key) = 0;
        virtual std::map<std::string, std::string>  GetHeaders(void) const = 0;
        virtual std::string GetBody(void) const = 0;

    public:
        virtual std::string CreateResponse(void) = 0;
        virtual std::string ToString(void) = 0;

        // Geters
        virtual std::string GetStatusName(HttpStatusCode::Code statusCode) = 0;
        virtual std::string GetTextContent(std::string extension) = 0;


        // Seters
        virtual ~IHttpResponse(void) {};

        // Seters
        virtual void SetStatusCode(HttpStatusCode::Code statusCode) = 0;
        virtual void SetHeader(std::string key, std::string value) = 0;
        virtual void SetBody(std::string body) = 0;


        friend class IBuilderResponse;
        friend class BuilderResponse;
};

#endif