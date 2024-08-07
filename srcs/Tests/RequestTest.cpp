# include <gtest/gtest.h>
# include "HttpRequest.hpp"

class RequestTest : public ::testing::Test {
protected:
    void SetUp() override {

    }
};

TEST_F(RequestTest, TestCreateAGETRequest) {
    // Arrange
    HttpRequest request;

    std::map<std::string, std::string> expectedHeaders;
    expectedHeaders["Host:"] = "www.example.com";
    expectedHeaders["User-Agent:"] = "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:89.0) Gecko/20100101 Firefox/89.0";
    expectedHeaders["Accept:"] = "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8";
    expectedHeaders["Accept-Language:"] = "en-US,en;q=0.5";
    expectedHeaders["Accept-Encoding:"] = "gzip, deflate, br";
    expectedHeaders["Connection:"] = "keep-alive";
    expectedHeaders["Upgrade-Insecure-Requests:"] = "1";
    std::stringstream requestMessage;
    requestMessage << "GET /index.html HTTP/1.1\r\n";
    std::map<std::string, std::string>::iterator it = expectedHeaders.begin();
    for ( ; it != expectedHeaders.end(); ++it) {
        requestMessage << it->first << " " << it->second << "\r\n";
    }
    requestMessage << "\r\n\r\n";

    // Act
    request.ParserRequest(requestMessage.str());

    // Assert
    EXPECT_EQ("GET", request.GetMethod());
    EXPECT_EQ("/index.html", request.GetPath());
    EXPECT_EQ("HTTP/1.1", request.GetHTTPVersion());
    std::map<std::string, std::string> headers = request.GetHeaders();
    std::map<std::string, std::string>::iterator itExp = expectedHeaders.begin();
    for (; itExp != expectedHeaders.end(); ++itExp) {
        EXPECT_NE(headers.find(itExp->first), headers.end());
        EXPECT_EQ(itExp->second, headers[itExp->first]);
    }
    EXPECT_EQ("", request.GetBody());
}

TEST_F(RequestTest, TestCreateAPOSTRequest) {
    // Arrange
    HttpRequest request;

    std::map<std::string, std::string> expectedHeaders;
    std::string expectedBody = "{\\n\\t\"username\": \"johndoe\",\\n\\t\"password\": \"securePassword123!\",\\n\\t\"email\": \"johndoe@example.com\"\\n}";
    expectedHeaders["Host:"] = "www.example.com";
    expectedHeaders["User-Agent:"] = "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:89.0) Gecko/20100101 Firefox/89.0";
    expectedHeaders["Accept:"] = "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8";
    expectedHeaders["Accept-Language:"] = "en-US,en;q=0.5";
    expectedHeaders["Accept-Encoding:"] = "gzip, deflate, br";
    expectedHeaders["Connection:"] = "keep-alive";
    expectedHeaders["Upgrade-Insecure-Requests:"] = "1";
    std::stringstream requestMessage;
    requestMessage << "POST /create-user HTTP/1.1\r\n";
    std::map<std::string, std::string>::iterator it = expectedHeaders.begin();
    for ( ; it != expectedHeaders.end(); ++it) {
        requestMessage << it->first << " " << it->second << "\r\n";
    }
    requestMessage << "\r\n";
    requestMessage << expectedBody << "\r\n";

    // Act
    request.ParserRequest(requestMessage.str());

    // Assert
    EXPECT_EQ("POST", request.GetMethod());
    EXPECT_EQ("/create-user", request.GetPath());
    EXPECT_EQ("HTTP/1.1", request.GetHTTPVersion());
    std::map<std::string, std::string> headers = request.GetHeaders();
    std::map<std::string, std::string>::iterator itExp = expectedHeaders.begin();
    for (; itExp != expectedHeaders.end(); ++itExp) {
        EXPECT_NE(headers.find(itExp->first), headers.end());
        EXPECT_EQ(itExp->second, headers[itExp->first]);
    }
    EXPECT_EQ(expectedBody, request.GetBody());
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}