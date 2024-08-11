#ifndef __IBUILDER_RESPONSE_HPP__
# define __IBUILDER_RESPONSE_HPP__
# include "HttpResponse.hpp"
# include "IHttpResponse.hpp"
# include "Handler.hpp"

class IBuilderResponse {
    public:
        virtual ~IBuilderResponse(void) {};

        virtual IBuilderResponse &SetupResponse(void) = 0;
        virtual IBuilderResponse &WithStatusCode(HttpStatusCode::Code statusCode) = 0;
        virtual IBuilderResponse &WithHeader(std::pair<std::string, std::string> pair) = 0;
        virtual IBuilderResponse &WithDirectoryFile(DIR *directory, std::string path) = 0;
        virtual IBuilderResponse &WithFileDescriptor(std::ifstream *fd) = 0;
        virtual IBuilderResponse &WithDefaultPage(void) = 0;
        virtual IBuilderResponse &WithContentType(std::string extension) = 0;
        virtual IBuilderResponse &WithLocation(std::string location) = 0;
        virtual IHttpResponse *GetResult(void) = 0;
        virtual bool CompareResponses(IHttpResponse &left, IHttpResponse &right) = 0;
};

#endif