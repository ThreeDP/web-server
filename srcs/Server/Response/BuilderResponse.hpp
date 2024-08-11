#ifndef __BUILDER_RESPONSE_HPP__
# define  __BUILDER_RESPONSE_HPP__

# include "IBuilderResponse.hpp"

class BuilderResponse : public IBuilderResponse {
    private:
        IHandler       *_handler;
        IHttpResponse  *_response;
    
    public:
        BuilderResponse(IHandler *handler);
        ~BuilderResponse(void);

        IBuilderResponse &SetupResponse(void);
        IBuilderResponse &WithStatusCode(HttpStatusCode::Code statusCode);
        IBuilderResponse &WithHeader(std::pair<std::string, std::string> pair);
        IBuilderResponse &WithDirectoryFile(DIR *directory, std::string path);
        IBuilderResponse &WithFileDescriptor(std::ifstream *fd);
        IBuilderResponse &WithDefaultPage(void);
        IBuilderResponse &WithContentType(std::string extension);
        IBuilderResponse &WithLocation(std::string location);
        IHttpResponse *GetResult(void);

        bool CompareResponses(IHttpResponse &left, IHttpResponse &right);
};

#endif