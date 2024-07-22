# include <gtest/gtest.h>
# include <iostream>
# include "IServer.hpp"
# include "Route.hpp"

class ServerTest : public IServer {

    private:
        std::string                             _listen_host;
        unsigned short                          _listen_port;
        std::vector<std::string>                _server_names;
        std::set<std::string>                   _default_allow_methods;
        std::map<int, std::string>              _default_error_page;
        int                                     _limit_client_body_size;
        std::map<std::string, std::string>      _rewrites;
        std::string                             _root;
        std::set<std::string>                   _index;
        bool                                    _autoindex;

    public:
        ServerTest(void) : 
            _listen_host("127.0.0.1"),
            _listen_port(8081),
            _limit_client_body_size(2048),
            _root("/app"),
            _autoindex(false) {
            _default_allow_methods.insert("GET");
            _default_allow_methods.insert("POST");
            _default_allow_methods.insert("DELETE");
            _default_error_page[404] = "404.html";
            _default_error_page[500] = "50x.html";
            _index.insert("index.html");
            _index.insert("index.php");
        }

        std::set<std::string>				*GetDefaultAllowMethods(void) {
            return &_default_allow_methods;
        }

        std::map<int, std::string>			&GetDefaultErrorPage(void) {
            return _default_error_page;
        }

        int 								GetLimitClientBodySize(void) {
            return _limit_client_body_size;
        }

        std::map<std::string, std::string>	&GetReWrites(void) {
            return _rewrites;
        }

        std::string 						GetRoot(void) {
            return _root;
        }

        std::set<std::string> 				&GetIndex(void) {
            return _index;
        }

        bool 								GetAutoIndex(void) {
            return _autoindex;
        }

        std::vector<std::string>			GetServerNames(void) {
            return _server_names;
        }
};

class RouteTest : public ::testing::Test {
protected:
    void SetUp() override {
        //AHttpResponse::SetDefaultAHTTPResponse();
    }
};

TEST_F(RouteTest, CreateARoute) {
    // Arrange
    std::string     server_name = "localhost";
    IServer         *server = new ServerTest();

    // Act
    Route route(server, server_name);

    // Assert
    EXPECT_EQ(route.GetRedirectPath(), "");
    EXPECT_EQ(route.GetErrorPage(404), "404.html");
    EXPECT_EQ(route.GetErrorPage(500), "50x.html");
    EXPECT_EQ(route.GetErrorPage(401), "");
    EXPECT_EQ(route.IsAllowMethod("GET"), true);
    EXPECT_EQ(route.IsAllowMethod("POST"), true);
    EXPECT_EQ(route.IsAllowMethod("DELETE"), true);
    EXPECT_EQ(route.IsAllowMethod("PUT"), false);
    EXPECT_EQ(route.GetLimitClientBodySize(), 2048);
    EXPECT_EQ(route.GetRoot(), "/app");
    std::set<std::string> indexs = route.GetFilesForIndex();
    EXPECT_NE(indexs.find("index.html"), indexs.end());
    EXPECT_NE(indexs.find("index.php"), indexs.end());
    EXPECT_EQ(indexs.find("index.py"), indexs.end());
}

TEST_F(RouteTest, CheckAAllowMethod) {
    // Arrange
    RouteResponse *expected = new RouteResponse(6, 200, false);
    HttpRequest request;
    request.ParserRequest(
        "GET /index.html HTTP/1.0\r\n"
    );
    std::string     server_name = "localhost";
    IServer         *server = new ServerTest();
    Route route(server, server_name);

    // Act
    RouteResponse *response = route.ProcessRequest(request);

    // Assert
    EXPECT_EQ(expected->fd, response->fd);
    EXPECT_EQ(expected->statusCode, response->statusCode);
    EXPECT_EQ(*expected, *response);
    delete response;
}

TEST_F(RouteTest, CheckNotAllowMethod) {
    // Arrange
    RouteResponse *expected = new RouteResponse(-1, 405, false);
    HttpRequest request;
    request.ParserRequest(
        "PUT /create HTTP/1.0\r\n"
    );
    std::string     server_name = "localhost";
    IServer         *server = new ServerTest();
    Route route(server, server_name);

    // Act
    RouteResponse *response = route.ProcessRequest(request);

    // Assert
    EXPECT_EQ(*expected, *response);
    delete response;
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}