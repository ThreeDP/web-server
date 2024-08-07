#ifndef __IAHTTP_RESPONSE_HPP__
# define __IAHTTP_RESPONSE_HPP__

class IAHttpResponse {
    protected:
        virtual std::string _defaultErrorPage(void) = 0;

    public:
        virtual ~IAHttpResponse() {}

        virtual std::string CreateResponse(void) = 0;
        virtual std::string GetStatusName(std::string statusCode) = 0;
        virtual std::string GetTextContent(std::string extension) = 0;
};

#endif