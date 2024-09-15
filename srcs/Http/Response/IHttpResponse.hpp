#ifndef __IHTTP_RESPONSE_HPP__
# define __IHTTP_RESPONSE_HPP__

# include "EHttpStatusCode.hpp"
# include "Handler.hpp"
# include "Logger.hpp"

class IBuilderResponse;

class IHttpResponse {
    private:
        virtual std::string     _toString(void) = 0;
        virtual void            _defaultErrorPage(void) = 0;
        virtual void            _createBodyByDirectory(std::set<std::string> dirent, std::string path, IHandler &handler) = 0;
        
        // Seters
        virtual void SetStatusCode(HttpStatusCode::Code statusCode) = 0;
        virtual void SetHeader(std::string key, std::string value) = 0;
        virtual void SetBody(std::string body) = 0;
        virtual void SetBody(std::vector<char> body) = 0;

        virtual std::pair<std::string, std::string> GetHeader(std::string key) = 0;
        virtual std::string                         GetHttpVersion(void) const = 0;
        virtual std::map<std::string, std::string>  GetHeaders(void) const = 0;

    public:
        // Geters
        virtual std::string             GetBody(void) const = 0;
        virtual ssize_t                 GetBodySize(void) const = 0;
        virtual std::string             GetStatusMessage(void) const = 0;
        virtual std::string             GetStatusCode(void) const = 0;
        virtual std::string             GetTextContent(std::string extension) = 0;

        virtual std::vector<char>       CreateResponse(void) = 0;

        virtual ~IHttpResponse(void) {};

        friend class BuilderResponse;
        friend class IBuilderResponse;
};

#endif