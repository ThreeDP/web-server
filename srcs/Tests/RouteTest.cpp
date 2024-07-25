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
        std::vector<std::string>                _route_names;
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
            _rewrites["/rewrite"] = "/new-path",
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
            return _route_names;
        }
};

IHandler            *handler;

class RouteTest : public ::testing::Test {
protected:
    void SetUp() override {
        handler = new HandlerTest();
    }
};

TEST_F(RouteTest, CreateARoute) {
    // Arrange
     
    std::string     route_name = "/rewrite";
    IServer         *server = new ServerTest();

    // Act
    Route route(server, route_name, handler);

    // Assert
    EXPECT_EQ("404.html", route.GetErrorPage(404));
    EXPECT_EQ("50x.html", route.GetErrorPage(500));
    EXPECT_EQ("", route.GetErrorPage(401));
    EXPECT_EQ(true, route.IsAllowMethod("GET"));
    EXPECT_EQ(true, route.IsAllowMethod("POST"));
    EXPECT_EQ(true, route.IsAllowMethod("DELETE"));
    EXPECT_EQ(false, route.IsAllowMethod("PUT"));
    EXPECT_EQ(250, route.GetLimitClientBodySize());
    EXPECT_EQ("/app", route.GetRoot());
    EXPECT_EQ("/new-path", route.GetRedirectPath());
    std::set<std::string> indexs = route.GetFilesForIndex();
    EXPECT_NE(indexs.find("index.html"), indexs.end());
    EXPECT_NE(indexs.find("index.php"), indexs.end());
    EXPECT_EQ(indexs.find("index.py"), indexs.end());
}

TEST_F(RouteTest, CheckAAllowMethod) {
    // Arrange
    std::stringstream   requestString;
    IServer             *server = new ServerTest();
    RouteResponse       *expected = new RouteResponse(6, 200);
    HttpRequest         request;
    std::string         route_name = "/index.html";
    Route               route(server, route_name, handler);

    requestString << "GET " << route_name << " HTTP/1.0\r\n";
    request.ParserRequest(requestString.str());

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
    IServer                     *server = new ServerTest();
    RouteResponse               *expected = new RouteResponse(-1, 405);
    HttpRequest                 request;
    std::string                 route_name = "/create";
    std::stringstream           requestString;
    Route                       route(server, route_name, handler);

    requestString << "PUT " << route_name << " HTTP/1.0\r\n";
    request.ParserRequest(requestString.str());

    // Act
    RouteResponse *response = route.ProcessRequest(request);

    // Assert
    EXPECT_EQ(*expected, *response);
    delete response;
}

TEST_F(RouteTest, CheckNotAllowMethodWithSetErrorPage) {
    // Arrange
     
    IServer                     *server = new ServerTest(405, "405.html");
    RouteResponse               *expected = new RouteResponse(6, 405);
    HttpRequest                 request;
    std::string                 route_name = "/create";
    Route                       route(server, route_name, handler);
    std::stringstream           requestString;

    requestString << "PUT " << route_name << " HTTP/1.0\r\n";
    request.ParserRequest(requestString.str());

    // Act
    RouteResponse *response = route.ProcessRequest(request);

    // Assert
    EXPECT_EQ(*expected, *response);
    delete response;
}

TEST_F(RouteTest, CheckAPayloadTooLarge) {
    // Arrange
    IServer                     *server = new ServerTest();
    RouteResponse               *expected = new RouteResponse(-1, 413);
    HttpRequest                 request;
    std::stringstream           body;
    std::string                 route_name = "/create-page";
    Route                       route(server, route_name, handler);
     
    body << "POST " << route_name << " HTTP/1.0\r\n";
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

    // Act
    RouteResponse *response = route.ProcessRequest(request);

    // Assert
    EXPECT_EQ(*expected, *response);
    delete response;
}

TEST_F(RouteTest, CheckAPayloadTooLargeWithErrorPageSetup) {
    // Arrange
    IServer                     *server = new ServerTest(413, "413.html");
    RouteResponse               *expected = new RouteResponse(6, 413);
    HttpRequest                 request;
    std::stringstream           body;
    std::string                 route_name = "/create-page";
    Route                       route(server, route_name, handler);

    body << "POST " << route_name << " HTTP/1.0\r\n";
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


    // Act
    RouteResponse *response = route.ProcessRequest(request);

    // Assert
    EXPECT_EQ(*expected, *response);
    delete response;
}

TEST_F(RouteTest, CheckARedirect) {
    // Arrange
    IServer                     *server = new ServerTest();
    RouteResponse               *expected = new RouteResponse(-1, 308, "/new-path");
    HttpRequest                 request;
    std::stringstream           requestString;
    std::string                 route_name = "/rewrite";
    Route                       route(server, route_name, handler);
    
    requestString << "PUT " << route_name << " HTTP/1.0\r\n";
    request.ParserRequest(requestString.str());

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