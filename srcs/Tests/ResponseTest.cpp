# include <gtest/gtest.h>
# include <iostream>
# include "AHttpResponse.hpp"
# include "Response200OK.hpp"
# include "Response404NotFound.hpp"

class ResponseTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
};

std::string HttpResponseTest(
    std::string statusCode,
    std::string statusMessage,
    std::string contentType) {

    std::ostringstream oss;
    std::ostringstream body;

    body << "<html data-lt-installed=\"true\">";
    body << "<head><title>" << statusCode << " " << statusMessage;
    body << "</title></head><body><center>";
    body << "<h1>" << statusCode << " " << statusMessage << "</h1>";
    body << "</center><hr><center>RavyServer/1.27.0</center>";
    body << "</body></html>\r\n";
    oss << "HTTP/1.1 " << statusCode << " " << statusMessage << "\r\n";
    oss << "Content-Length: " << body.str().length() << "\r\n";
    oss << "Content-Type: " << contentType << "\r\n";
    oss << "Date: " << Utils::getCurrentTimeInGMT() << "\r\n";
    oss << "\r\n";
    oss << body.str();

    return oss.str();
}

std::string HttpResponseTest(
    std::string statusCode,
    std::string statusMessage,
    std::string contentType,
    std::string body) {

    std::ostringstream oss;

    oss << "HTTP/1.1 " << statusCode << " " << statusMessage << "\r\n";
    oss << "Content-Length: " << body.length() + 2 << "\r\n";
    oss << "Content-Type: " << contentType << "\r\n";
    oss << "Date: " << Utils::getCurrentTimeInGMT() << "\r\n";
    oss << "\r\n";
    oss << body << "\r\n";

    return oss.str();
}

TEST(TestResponse200OkWithoutBody, Response200OK) {
    // Arrange
    AHttpResponse::SetDefaultAHTTPResponse();
    std::string     expectedResponse = HttpResponseTest(
        "200", "OK", "text/html; charset=utf-8"
    );

    // Act
    Response200OK response(".html");

    // Assert
    EXPECT_EQ(expectedResponse, response.CreateResponse());
}

TEST(TestResponse200OkWithBody, Response200OK) {
    // Arrange
    AHttpResponse::SetDefaultAHTTPResponse();
    std::string     expectedResponse = HttpResponseTest(
        "200", "OK", "text/html; charset=utf-8", "<h1>200</h1>");

    // Act
    Response200OK response(".html", "<h1>200</h1>");

    // Assert
    EXPECT_EQ(expectedResponse, response.CreateResponse());
}