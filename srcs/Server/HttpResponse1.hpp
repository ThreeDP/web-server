// #ifndef __HTTP_RESPONSE_HPP__
// # define __HTTP_RESPONSE_HPP__

// # include "RouteResponse.hpp"

// # include <iomanip>
// # include <ctime>
// # include <map>
// # include <cstring>

// class HttpResponse {
//     protected:
//         std::string                         _HTTPVersion;
//         std::string                         _StatusCode;
//         std::string                         _Status;
//         std::map<std::string, std::string>  _payload;
//         std::string                         _Body;

//     public:
//         // Base Methods
//         HttpResponse();
//         ~HttpResponse();
//         // HttpResponse(RouteResponse &rr);
//         // makHttpResponse(std::string body, std::string statusCode, std::string contentType);

//         // Create Response
//         std::string             CreateResponse(void);
//         std::string             GetTimeNow(void);
// };

// #endif