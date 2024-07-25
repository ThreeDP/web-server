# include <gtest/gtest.h>
# include <iostream>
# include "IServer.hpp"
# include "Route.hpp"

class HandlerTest : public IHandler {
    int                     OpenFile(std::string path) {
        if (path != "") {
            return 6;
        }
        return -1;
    }

};

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
            _limit_client_body_size(250),
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

        ServerTest(int statusCode, std::string path) : 
            _listen_host("127.0.0.1"),
            _listen_port(8081),
            _limit_client_body_size(250),
            _root("/app"),
            _autoindex(false) {
            _default_allow_methods.insert("GET");
            _default_allow_methods.insert("POST");
            _default_allow_methods.insert("DELETE");
            _default_error_page[404] = "404.html";
            _default_error_page[500] = "50x.html";
            _default_error_page[statusCode] = path;
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
    IHandler *handler = new HandlerTest(); 
    std::string     server_name = "localhost";
    IServer         *server = new ServerTest();

    // Act
    Route route(server, server_name, handler);

    // Assert
    EXPECT_EQ(route.GetRedirectPath(), "");
    EXPECT_EQ(route.GetErrorPage(404), "404.html");
    EXPECT_EQ(route.GetErrorPage(500), "50x.html");
    EXPECT_EQ(route.GetErrorPage(401), "");
    EXPECT_EQ(route.IsAllowMethod("GET"), true);
    EXPECT_EQ(route.IsAllowMethod("POST"), true);
    EXPECT_EQ(route.IsAllowMethod("DELETE"), true);
    EXPECT_EQ(route.IsAllowMethod("PUT"), false);
    EXPECT_EQ(route.GetLimitClientBodySize(), 250);
    EXPECT_EQ(route.GetRoot(), "/app");
    std::set<std::string> indexs = route.GetFilesForIndex();
    EXPECT_NE(indexs.find("index.html"), indexs.end());
    EXPECT_NE(indexs.find("index.php"), indexs.end());
    EXPECT_EQ(indexs.find("index.py"), indexs.end());
}

TEST_F(RouteTest, CheckAAllowMethod) {
    // Arrange
    IHandler *handler = new HandlerTest(); 
    RouteResponse *expected = new RouteResponse(6, 200, false);
    HttpRequest request;
    request.ParserRequest(
        "GET /index.html HTTP/1.0\r\n"
    );
    std::string     server_name = "localhost";
    IServer         *server = new ServerTest();
    Route route(server, server_name, handler);

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
    IHandler *handler = new HandlerTest(); 
    RouteResponse *expected = new RouteResponse(-1, 405, false);
    HttpRequest request;
    request.ParserRequest(
        "PUT /create HTTP/1.0\r\n"
    );
    std::string     server_name = "localhost";
    IServer         *server = new ServerTest();
    Route route(server, server_name, handler);

    // Act
    RouteResponse *response = route.ProcessRequest(request);

    // Assert
    EXPECT_EQ(*expected, *response);
    delete response;
}

TEST_F(RouteTest, CheckNotAllowMethodWithSetErrorPage) {
    // Arrange
    IHandler *handler = new HandlerTest(); 
    RouteResponse *expected = new RouteResponse(6, 405, false);
    HttpRequest request;
    request.ParserRequest(
        "PUT /create HTTP/1.0\r\n"
    );
    std::string     server_name = "localhost";
    IServer         *server = new ServerTest(405, "405.html");
    Route route(server, server_name, handler);

    // Act
    RouteResponse *response = route.ProcessRequest(request);

    // Assert
    EXPECT_EQ(*expected, *response);
    delete response;
}

TEST_F(RouteTest, CheckAPayloadTooLarge) {
    // Arrange
    IHandler *handler = new HandlerTest(); 
    RouteResponse *expected = new RouteResponse(-1, 413, false);
    HttpRequest request;
    std::stringstream body;
    body << "POST /create-page HTTP/1.0\r\n";
    body << "\r\n";
    body << "<!DOCTYPE html>\n";
    body << "<html lang=\"pt-BR\">\n";
    body << "<head>\n";
    body << "<meta charset=\"UTF-8\">\n";
    body << "<title>Página Compacta</title>\n"; 
    body << "<style>\n";
    body << "body{font-family:Arial,sans-serif;background-color:#f4f4f9;color:#333;padding:10px;}\n";
    body << "a{color:#0066cc;text-decoration:none;}\n";
    body << "</style>\n";
    body << "</head>\n";
    body << "<body>\n";
    body << "<h1>Bem-vindo!</h1>\n"; 
    body << "<p>Esta é uma página HTML de 350 caracteres. Ela contém texto, estilo e um link.</p>\n";
    body << "<a href=\"https://www.exemplo.com\">Clique aqui para mais informações</a>\n";
    body << "</body>\n";
    body << "</html>\r\n";
    request.ParserRequest(body.str());

    std::string     server_name = "localhost";
    IServer         *server = new ServerTest();
    Route route(server, server_name, handler);

    // Act
    RouteResponse *response = route.ProcessRequest(request);

    // Assert
    EXPECT_EQ(*expected, *response);
    delete response;
}

TEST_F(RouteTest, CheckAPayloadTooLargeWithErrorPageSetup) {
    // Arrange
    IHandler *handler = new HandlerTest(); 
    RouteResponse *expected = new RouteResponse(6, 413, false);
    HttpRequest request;
    std::stringstream body;
    body << "POST /create-page HTTP/1.0\r\n";
    body << "\r\n";
    body << "<!DOCTYPE html>\n";
    body << "<html lang=\"pt-BR\">\n";
    body << "<head>\n";
    body << "<meta charset=\"UTF-8\">\n";
    body << "<title>Página Compacta</title>\n"; 
    body << "<style>\n";
    body << "body{font-family:Arial,sans-serif;background-color:#f4f4f9;color:#333;padding:10px;}\n";
    body << "a{color:#0066cc;text-decoration:none;}\n";
    body << "</style>\n";
    body << "</head>\n";
    body << "<body>\n";
    body << "<h1>Bem-vindo!</h1>\n"; 
    body << "<p>Esta é uma página HTML de 350 caracteres. Ela contém texto, estilo e um link.</p>\n";
    body << "<a href=\"https://www.exemplo.com\">Clique aqui para mais informações</a>\n";
    body << "</body>\n";
    body << "</html>\r\n";
    request.ParserRequest(body.str());

    std::string     server_name = "localhost";
    IServer         *server = new ServerTest(413, "413.html");
    Route route(server, server_name, handler);

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