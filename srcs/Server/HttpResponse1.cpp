// # include "HttpResponse.hpp"

// /* Create Response
// =============================================*/
// std::string HttpResponse::CreateResponse(void) {
//     std::stringstream response;

//     response << this->_HTTPVersion << " " << this->_StatusCode << " " << this->_Status << "\r\n";
//     std::map<std::string, std::string>::iterator it = this->_payload.begin();
//     for (; it != this->_payload.end(); ++it) {
//         response << it->first << " " << it->second << "\r\n";
//     }
//     response << "\r\n" << this->_Body << "\r\n";
//     return response.str();
// }

// std::string HttpResponse::GetTimeNow(void) {
//     char buffer[80];
//     memset(&buffer, '\0', sizeof(char) * 80);
//     std::time_t now = std::time(NULL);
//     std::tm* gmt_time = std::gmtime(&now);
//     std::stringstream ss;

//     std::strftime(buffer, sizeof(char) * 80, "%Y-%m-%d %H:%M:%S GMT", gmt_time);
//     ss << buffer;
//     return ss.str();
// }

// /* Base Method
// =============================================*/
// HttpResponse::HttpResponse(void) {

// }

// HttpResponse::~HttpResponse(void) {

// }

// // HttpResponse::HttpResponse(std::string body, std::string statusCode, std::string contentType)
// //     : _HTTPVersion("HTTP/1.1"), _StatusCode(statusCode),
// //     _Status("OK") {
// //     this->_payload["Content-Type:"] = contentType;
// //     std::stringstream num;
// //     num << body.size() + 2;
// //     this->_payload["Content-Length:"] = std::string(num.str());
// //     this->_payload["Date:"] = this->GetTimeNow();
// //     this->_Body = body;
// // }

// // HttpResponse::HttpResponse(RouteResponse &rr)
// //     : _HTTPVersion("HTTP/1.1"), _StatusCode(rr.getStatusCode()),
// //     _Status(rr.getStatusName()) {
// //     this->_payload["Content-Type:"] = rr.getContentType();
// //     std::stringstream num;
// //     num << rr.getBody().size() + 2;
// //     this->_payload["Content-Length:"] = std::string(num.str());
// //     this->_payload["Date:"] = this->GetTimeNow();
// //     this->_Body = rr.getBody();
// // }