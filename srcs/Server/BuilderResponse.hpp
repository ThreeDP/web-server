#ifndef __BUILDER_RESPONSE_HPP__
# define  __BUILDER_RESPONSE_HPP__

# include "IBuilderResponse.hpp"

class BuilderResponse : public IBuilderResponse {
    private:
        IHandler       *_handler;
        IHttpResponse   *_response;
    
    public:
        BuilderResponse(IHandler *handler) : _handler(handler) {
            _handler = handler;
        }

        ~BuilderResponse(void) {
            if (_response != NULL)
                delete _response;
        }

        void Setup(void) {
            if (_response != NULL)
                delete _response;
            _response = new HttpResponse();
        }

        IBuilderResponse &WithStatusCode(HttpStatusCode::Code statusCode) {
            _response->SetStatusCode(statusCode);
            return *this; 
        }

        IBuilderResponse &WithHeader(std::pair<std::string, std::string> pair) {
            _response->SetHeader(pair.first, pair.second);
            return *this;
        }

        IBuilderResponse &WithDirectoryFile(DIR *directory) {
            if (directory != NULL && _response->GetBody() == "") {
                std::vector<struct dirent*> *directories = _handler->ReadDirectory(directory);
                _response->_createBodyByDirectory(directories);
            }
            closedir(directory);
            return *this;
        }

        IBuilderResponse &WithFileDescriptor(std::ifstream *fd) {
            if (fd != NULL && _response->GetBody() == "") {
                _response->SetBody(_handler->ReadRegularFile(fd));
            }
            fd->close();
            delete fd;
            return *this;
        }

        IBuilderResponse &WithDefaultPage(void) {
            if (_response->GetBody() == "") {
                _response->_defaultErrorPage();
            }
            return *this;
        }

        IBuilderResponse &WithContentType(std::string extension) {
            _response->SetHeader("Content-Type:", _response->GetTextContent(extension));
            return *this; 
        };

        IBuilderResponse &WithLocation(std::string location) {
            _response->SetHeader("Location:", location);
            return *this;
        };

        IHttpResponse *GetResult(void) {
            return _response;
        }
};

#endif