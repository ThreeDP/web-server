# include <gtest/gtest.h>
# include <iostream>
# include "HttpResponse.hpp"

class ResponseTest : public ::testing::Test {
protected:
    void SetUp() override {
        HttpResponse::SetDefaultHTTPResponse();
    }
};

std::string HeaderCreate(
    std::map<std::string, std::string> headers,
    std::string body,
    std::string statusCode,
    std::string statusMessage,
    std::string contentType) {
    std::ostringstream oss;
    std::ostringstream bodySize;
    std::ostringstream dateGMT;

    bodySize << body.length() + 2;
    dateGMT << Utils::getCurrentTimeInGMT();
    oss << "HTTP/1.1 " << statusCode << " " << statusMessage << "\r\n";
    headers["Content-Length:"] = bodySize.str();
    headers["Content-Type:"] = contentType;
    headers["Date:"] = dateGMT.str();
    headers["Server:"] = "Ravy 1.0.0";
    for (std::map<std::string, std::string>::iterator it = headers.begin();
        it != headers.end(); ++it) {
        oss << it->first << " " << it->second << "\r\n";
    }
    oss << "\r\n";
    return oss.str();
}

std::string BodyCreate(
    std::string statusCode,
    std::string statusMessage) {
    std::ostringstream body;

    body << "<html data-lt-installed=\"true\">";
    body << "<head><title>" << statusCode << " " << statusMessage;
    body << "</title></head><body><center>";
    body << "<h1>" << statusCode << " " << statusMessage << "</h1>";
    body << "</center><hr><center>Ravy 1.0.0</center>";
    body << "</body></html>";
    return body.str();
}

std::string HttpResponseTest(
    std::map<std::string, std::string> headers,
    std::string statusCode,
    std::string statusMessage,
    std::string contentType,
    std::string body) {

    std::ostringstream oss;

    oss << HeaderCreate(headers, body, statusCode, statusMessage, contentType);
    oss << body << "\r\n";

    return oss.str();
}

// TEST_F(ResponseTest, TestResponse200OkWithoutBody) {
//     // Arrange
//     std::map<std::string, std::string> extraHeaders;
//     std::string     expectedResponse = HttpResponseTest(
//         extraHeaders,
//         "200",
//         "OK",
//         "text/html; charset=utf-8",
//         BodyCreate("200", "OK")
//     );

//     // Act
//     IAHttpResponse *response = new Response200OK(".html");

//     // Assert
//     EXPECT_EQ(expectedResponse, response->CreateResponse());
//     delete response;
// }

// TEST_F(ResponseTest, TestResponse200OkWithBody) {
//     // Arrange
//     std::map<std::string, std::string> extraHeaders;
//     std::string     expectedResponse = HttpResponseTest(
//         extraHeaders,
//         "200",
//         "OK",
//         "text/html; charset=utf-8",
//         "<h1>200</h1>");

//     // Act
//     IAHttpResponse *response = new Response200OK(".html", "<h1>200</h1>");

//     // Assert
//     EXPECT_EQ(expectedResponse, response->CreateResponse());
//     delete response;
// }

// TEST_F(ResponseTest, TestResponse404NotFoundWithoutBody) {
//     // Arrange
//     std::map<std::string, std::string> extraHeaders;
//     std::string     expectedResponse = HttpResponseTest(
//         extraHeaders,
//         "404",
//         "Not Found",
//         "text/html; charset=utf-8",
//         BodyCreate("404", "Not Found")
//     );

//     // Act
//     IAHttpResponse *response = new Response404NotFound(".html");

//     // Assert
//     EXPECT_EQ(expectedResponse, response->CreateResponse());
//     delete response;
// }

// TEST_F(ResponseTest, TestResponse404NotFoundWithBody) {
//     // Arrange
//     std::map<std::string, std::string> extraHeaders;
//     std::string     expectedResponse = HttpResponseTest(
//         extraHeaders,
//         "404",
//         "Not Found",
//         "text/html; charset=utf-8",
//         "<h1>404</h1>"
//     );

//     // Act
//     IAHttpResponse *response = new Response404NotFound(
//         ".html", "<h1>404</h1>"
//     );

//     // Assert
//     EXPECT_EQ(expectedResponse, response->CreateResponse());
//     delete response;
// }

// TEST_F(ResponseTest, TestResponse201CreatedWithoutBody) {
//     // Arrange
//     std::map<std::string, std::string> extraHeaders;
//     extraHeaders["Location:"] = "http://site.com/novo-item-1289";
//     std::string     expectedResponse = HttpResponseTest(
//         extraHeaders,
//         "201",
//         "Created",
//         "text/html; charset=utf-8",
//         BodyCreate("201", "Created")
//     );

//     // Act
//     IAHttpResponse *response = new Response201Created(
//         ".html", "http://site.com/novo-item-1289"
//     );

//     // Assert
//     EXPECT_EQ(expectedResponse, response->CreateResponse());
//     delete response;
// }

// TEST_F(ResponseTest, TestResponse201CreatedWithBody) {
//     // Arrange
//     std::map<std::string, std::string> extraHeaders;
//     extraHeaders["Location:"] = "http://site.com/novo-item-1289";
//     std::string     expectedResponse = HttpResponseTest(
//         extraHeaders,
//         "201",
//         "Created",
//         "text/html; charset=utf-8",
//         "<h1>201</h1>"
//     );

//     // Act
//     IAHttpResponse *response = new Response201Created(
//         ".html", "http://site.com/novo-item-1289", "<h1>201</h1>"
//     );

//     // Assert
//     EXPECT_EQ(expectedResponse, response->CreateResponse());
//     delete response;
// }

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}