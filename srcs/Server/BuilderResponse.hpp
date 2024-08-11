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
            _response = NULL;
        }

        ~BuilderResponse(void) {
            if (_response != NULL) {
                delete _response;
                _response = NULL;
            }
        }

        IBuilderResponse &SetupResponse(void) {
            if (_response != NULL) {
                delete _response;
                _response = NULL;
            }
            _response = new HttpResponse();
            return *this;
        }

        IBuilderResponse &WithStatusCode(HttpStatusCode::Code statusCode) {
            _response->SetStatusCode(statusCode);
            return *this; 
        }

        IBuilderResponse &WithHeader(std::pair<std::string, std::string> pair) {
            _response->SetHeader(pair.first, pair.second);
            return *this;
        }

        IBuilderResponse &WithDirectoryFile(DIR *directory, std::string path) {
            if (directory != NULL && _response->GetBody() == "") {
                std::vector<struct dirent*> *directories = _handler->ReadDirectory(directory);
                _response->_createBodyByDirectory(directories, path);
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
            IHttpResponse *res = _response;
            _response = NULL;
            return res;
        }

        bool CompareResponses(IHttpResponse &left, IHttpResponse &right) {
            bool res = left.GetHttpVersion() == right.GetHttpVersion()
                && left.GetStatusCode() == right.GetStatusCode()
                && left.GetStatusMessage() == right.GetStatusMessage();
            std::map<std::string, std::string> leftHeaders = left.GetHeaders();
            std::map<std::string, std::string> rightHeaders = right.GetHeaders();
            std::map<std::string, std::string>::iterator it = leftHeaders.begin();
            for ( ; it != leftHeaders.end(); ++it) {
                res = res && it->second == rightHeaders[it->first];
            }
            std::cout << "Left: " << left.GetBody() << "\n\n";
            std::cout << "Right: " << right.GetBody() << "\n\n";
            return res && left.GetBody() == right.GetBody();
        }
};

#endif