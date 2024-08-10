# include <gtest/gtest.h>
# include <iostream>
# include <sys/types.h>
# include <dirent.h>
# include "IServer.hpp"
# include "Route.hpp"
# include "Handler.hpp"
# include "BuilderResponse.hpp"

enum class CONFIGS {
    NoMethods,
    WithAutoIndex,
    WithAutoIndexWithoutIndex
};

class HandlerTest : public IHandler {

    public:
        HandlerTest(){}

        DIR *OpenDirectory(std::string path) {
            DIR *dir = (DIR *)calloc(1, sizeof(DIR *));
            return dir;
        }

        std::ifstream   *OpenFile(std::string path) {
            if (path != "")
                return new std::ifstream();
            return NULL;
        }

        bool    FileExist(std::string path) {
            if (path == "/app/notfound.html") {
                return false;
            }
            return true;
        }

        bool    FileIsDirectory(std::string path) {
            if (path == "/app/home")
                return true;
            if (path == "/app/notallow")
                return true;
            return false;
        }

        bool    FileIsRegular(std::string path) {
            return true;
        }

        bool    IsAllowToGetFile(std::string path) {
            if (path == "/app/notallow.html")
                return false;
            if (path == "/app/notallow")
                return false;
            return true;
        }

        bool PathExist(std::string path) {
            if (path == "/app/notfound.html") {
                return false;
            }
            if (path == "/app/DirectoryNotFound")
                return false;
            return true;
        }

        bool    IsAllowToRunScript(std::string path) {
            return true;
        }

        std::vector<struct dirent *>   *ReadDirectory(DIR *directory) {
            return new std::vector<struct dirent *>();
        }

        std::string ReadRegularFile(std::ifstream *file) {
            return "";
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

        ServerTest(CONFIGS flag) : 
            _listen_host("127.0.0.1"),
            _listen_port(8081),
            _limit_client_body_size(250),
            _root("/app"),
            _autoindex(false) {
            _rewrites["/rewrite"] = "/new-path";
            if (flag == CONFIGS::WithAutoIndex || flag == CONFIGS::WithAutoIndexWithoutIndex)
                _autoindex = true;
            if (flag != CONFIGS::NoMethods) {
                _default_allow_methods.insert("GET");
                _default_allow_methods.insert("POST");
                _default_allow_methods.insert("DELETE");
            }
            _default_error_page[404] = "404.html";
            _default_error_page[500] = "50x.html";
            if (flag != CONFIGS::WithAutoIndexWithoutIndex) {
                _index.insert("index.html");
                _index.insert("index.php");
            }
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

        void SetErroPage(int statusCode, std::string path) {
            this->_default_error_page[statusCode] = path;
        }

        void ResetErroPage(int statusCode) {
            this->_default_error_page[statusCode] = "";
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

ServerTest *ServerTestDefault;
ServerTest *ServerTestWithoutGet;
ServerTest *ServerTestWithAutoIndex;
ServerTest *ServerTestWithAutoIndexWithoutIndex;

IHandler            *handler;

class RouteTest : public ::testing::Test {
protected:
    void SetUp() override {
        handler = new HandlerTest();
        ServerTestDefault = new ServerTest();
        ServerTestWithoutGet = new ServerTest(CONFIGS::NoMethods);
        ServerTestWithAutoIndex = new ServerTest(CONFIGS::WithAutoIndex);
        ServerTestWithAutoIndexWithoutIndex = new ServerTest(CONFIGS::WithAutoIndexWithoutIndex);
    }
};

TEST_F(RouteTest, CreateARoute) {
    // Arrange
     
    std::string     route_name = "/rewrite";

    // Act
    Route route(ServerTestDefault, route_name, handler);

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

TEST_F(RouteTest, CheckAAllowMethod_CheckContentTypeIsHTML) {
    // Arrange
    BuilderResponse     builder = BuilderResponse(handler);
    builder.Setup();
    std::string         route_name = "/imagens";
    Route               route(ServerTestDefault, route_name, handler);

    HttpRequest         request;
    std::stringstream   requestString;
    requestString << "GET " << route_name << "/index.html" << " HTTP/1.0\r\n";
    request.ParserRequest(requestString.str());

    // Act
    HttpStatusCode::Code res = route.ProcessRequest(request, builder);

    // Assert
    EXPECT_EQ(HttpStatusCode::_OK, res);
    EXPECT_EQ("test", builder.GetResult()->GetStatusName(HttpStatusCode::_OK));
    // EXPECT_NE(response->FD, nullptr);
    // EXPECT_EQ(response->Directory, nullptr);

    // // Clean
    // delete expected;
    // response->FD->close();
    // delete response->FD;
    // delete response;
}

// TEST_F(RouteTest, CheckNotAllowMethod) {
//     // Arrange
//     RouteResponse               *expected = new RouteResponse(".html", nullptr, 405);
//     HttpRequest                 request;
//     std::string                 route_name = "/create";
//     std::stringstream           requestString;
//     Route                       route(ServerTestWithoutGet, route_name, handler);

//     requestString << "GET " << route_name << "/index.html" << " HTTP/1.0\r\n";
//     request.ParserRequest(requestString.str());

//     // Act
//     RouteResponse *response = route.ProcessRequest(request);

//     // Assert
//     EXPECT_EQ(*expected, *response);
//     EXPECT_EQ(response->FD, nullptr);
//     EXPECT_EQ(response->Directory, nullptr);

//     // Clean
//     delete expected;
//     delete response;
// }

// TEST_F(RouteTest, CheckNotAllowMethodWithSetErrorPage) {
//     // Arrange
//     ServerTestWithoutGet->SetErroPage(405, "405.html");
//     RouteResponse               *expected = new RouteResponse(".html", nullptr, 405);
//     HttpRequest                 request;
//     std::string                 route_name = "/create";
//     Route                       route(ServerTestWithoutGet, route_name, handler);
//     std::stringstream           requestString;

//     requestString << "GET " << route_name << "/index.html" << " HTTP/1.0\r\n";
//     request.ParserRequest(requestString.str());

//     // Act
//     RouteResponse *response = route.ProcessRequest(request);

//     // Assert
//     EXPECT_EQ(*expected, *response);
//     EXPECT_NE(response->FD, nullptr);
//     EXPECT_EQ(response->Directory, nullptr);

//     // Clean
//     ServerTestWithoutGet->ResetErroPage(405);
//     delete expected;
//     response->FD->close();
//     delete response->FD;
//     delete response;
// }

// TEST_F(RouteTest, CheckNotAllowMethodWithNotFoundErrorPage) {
//     // Arrange
//     ServerTestWithoutGet->SetErroPage(405, "notfound.html");
//     RouteResponse               *expected = new RouteResponse(".html", nullptr, 405);
//     HttpRequest                 request;
//     std::string                 route_name = "/page";
//     Route                       route(ServerTestWithoutGet, route_name, handler);
//     std::stringstream           requestString;

//     requestString << "GET " << route_name << "index.html" << " HTTP/1.0\r\n";
//     request.ParserRequest(requestString.str());

//     // Act
//     RouteResponse *response = route.ProcessRequest(request);

//     // Assert
//     EXPECT_EQ(*expected, *response);
//     EXPECT_EQ(response->FD, nullptr);
//     EXPECT_EQ(response->Directory, nullptr);

//     // Clean
//     ServerTestWithoutGet->ResetErroPage(405);
//     delete expected;
//     delete response;
// }

// TEST_F(RouteTest, CheckAPayloadTooLarge) {
//     // Arrange
//     RouteResponse               *expected = new RouteResponse(".html", nullptr, 413);
//     HttpRequest                 request;
//     std::stringstream           body;
//     std::string                 route_name = "/create-page";
//     Route                       route(ServerTestDefault, route_name, handler);
     
//     body << "GET " << route_name << "index.html" << " HTTP/1.0\r\n";
//     body << "\r\n";
//     body << "<!DOCTYPE html>\n";
//     body << "<html lang=\"pt-BR\">\n";
//     body << "<head>\n";
//     body << "<meta charset=\"UTF-8\">\n";
//     body << "<title>Página Compacta</title>\n"; 
//     body << "<style>\n";
//     body << "body{font-family:Arial,sans-serif;background-color:#f4f4f9;color:#333;padding:10px;}\n";
//     body << "a{color:#0066cc;text-decoration:none;}\n";
//     body << "</style>\n";
//     body << "</head>\n";
//     body << "<body>\n";
//     body << "<h1>Bem-vindo!</h1>\n"; 
//     body << "<p>Esta é uma página HTML de 350 caracteres. Ela contém texto, estilo e um link.</p>\n";
//     body << "<a href=\"https://www.exemplo.com\">Clique aqui para mais informações</a>\n";
//     body << "</body>\n";
//     body << "</html>\r\n";
//     request.ParserRequest(body.str());

//     // Act
//     RouteResponse *response = route.ProcessRequest(request);

//     // Assert
//     EXPECT_EQ(*expected, *response);
//     EXPECT_EQ(response->FD, nullptr);
//     EXPECT_EQ(response->Directory, nullptr);

//     // Clean
//     delete expected;
//     delete response;
// }

// TEST_F(RouteTest, CheckAPayloadTooLargeWithErrorPageSetup) {
//     // Arrange
//     ServerTestDefault->SetErroPage(413, "413.html");
//     RouteResponse               *expected = new RouteResponse(".html", nullptr, 413);
//     HttpRequest                 request;
//     std::stringstream           body;
//     std::string                 route_name = "/create-page";
//     Route                       route(ServerTestDefault, route_name, handler);

//     body << "GET " << route_name << "/index.html" << " HTTP/1.0\r\n";
//     body << "\r\n";
//     body << "<!DOCTYPE html>\n";
//     body << "<html lang=\"pt-BR\">\n";
//     body << "<head>\n";
//     body << "<meta charset=\"UTF-8\">\n";
//     body << "<title>Página Compacta</title>\n"; 
//     body << "<style>\n";
//     body << "body{font-family:Arial,sans-serif;background-color:#f4f4f9;color:#333;padding:10px;}\n";
//     body << "a{color:#0066cc;text-decoration:none;}\n";
//     body << "</style>\n";
//     body << "</head>\n";
//     body << "<body>\n";
//     body << "<h1>Bem-vindo!</h1>\n"; 
//     body << "<p>Esta é uma página HTML de 350 caracteres. Ela contém texto, estilo e um link.</p>\n";
//     body << "<a href=\"https://www.exemplo.com\">Clique aqui para mais informações</a>\n";
//     body << "</body>\n";
//     body << "</html>\r\n";
//     request.ParserRequest(body.str());

//     // Act
//     RouteResponse *response = route.ProcessRequest(request);

//     // Assert
//     EXPECT_EQ(*expected, *response);
//     EXPECT_NE(response->FD, nullptr);
//     EXPECT_EQ(response->Directory, nullptr);

//     // Clean
//     delete expected;
//     ServerTestDefault->ResetErroPage(413);
//     response->FD->close();
//     delete response->FD;
//     delete response;
// }

// TEST_F(RouteTest, CheckAPayloadTooLargeWithErrorPageSetupNotFound) {
//     // Arrange
//     ServerTestDefault->SetErroPage(413, "/notfound.html");
//     RouteResponse               *expected = new RouteResponse(".html", nullptr, 413);
//     HttpRequest                 request;
//     std::stringstream           body;
//     std::string                 route_name = "/";
//     Route                       route(ServerTestDefault, route_name, handler);

//     body << "GET " << route_name << " HTTP/1.0\r\n";
//     body << "\r\n";
//     body << "<!DOCTYPE html>\n";
//     body << "<html lang=\"pt-BR\">\n";
//     body << "<head>\n";
//     body << "<meta charset=\"UTF-8\">\n";
//     body << "<title>Página Compacta</title>\n"; 
//     body << "<style>\n";
//     body << "body{font-family:Arial,sans-serif;background-color:#f4f4f9;color:#333;padding:10px;}\n";
//     body << "a{color:#0066cc;text-decoration:none;}\n";
//     body << "</style>\n";
//     body << "</head>\n";
//     body << "<body>\n";
//     body << "<h1>Bem-vindo!</h1>\n"; 
//     body << "<p>Esta é uma página HTML de 350 caracteres. Ela contém texto, estilo e um link.</p>\n";
//     body << "<a href=\"https://www.exemplo.com\">Clique aqui para mais informações</a>\n";
//     body << "</body>\n";
//     body << "</html>\r\n";
//     request.ParserRequest(body.str());

//     // Act
//     RouteResponse *response = route.ProcessRequest(request);

//     // Assert
//     EXPECT_EQ(*expected, *response);
//     EXPECT_EQ(response->FD, nullptr);
//     EXPECT_EQ(response->Directory, nullptr);

//     // Clean
//     ServerTestDefault->ResetErroPage(413);
//     delete expected;
//     delete response;
// }

// TEST_F(RouteTest, CheckARedirect) {
//     // Arrange
//     RouteResponse               *expected = new RouteResponse(nullptr, 308, "/new-path");
//     HttpRequest                 request;
//     std::stringstream           requestString;
//     std::string                 route_name = "/rewrite";
//     Route                       route(ServerTestDefault, route_name, handler);
    
//     requestString << "GET " << route_name << " HTTP/1.0\r\n";
//     request.ParserRequest(requestString.str());

//     // Act
//     RouteResponse *response = route.ProcessRequest(request);

//     // Assert
//     EXPECT_EQ(*expected, *response);
//     EXPECT_EQ(response->FD, nullptr);
//     EXPECT_EQ(response->Directory, nullptr);

//     // Clean
//     delete expected;
//     delete response;
// }

// TEST_F(RouteTest, CheckANotFoundFile) {
//     // Arrange
//     RouteResponse               *expected = new RouteResponse(".html", nullptr, 404);
//     HttpRequest                 request;
//     std::stringstream           requestString;
//     std::string                 route_name = "/";
//     Route                       route(ServerTestDefault, route_name, handler);
    
//     requestString << "GET " << route_name << "notfound.html" << " HTTP/1.0\r\n";
//     request.ParserRequest(requestString.str());

//     // Act
//     RouteResponse *response = route.ProcessRequest(request);

//     // Assert
//     EXPECT_EQ(*expected, *response);
//     EXPECT_NE(response->FD, nullptr);
//     EXPECT_EQ(response->Directory, nullptr);

//     // Clean
//     response->FD->close();
//     delete response->FD;
//     delete response;
// }

// TEST_F(RouteTest, CheckANotFoundFileWithNotFoundErrorPage) {
//     // Arrange
//     ServerTestDefault->SetErroPage(404, "notfound.html");
//     RouteResponse               *expected = new RouteResponse(".html", nullptr, 404);
//     HttpRequest                 request;
//     std::stringstream           requestString;
//     std::string                 route_name = "/";
//     Route                       route(ServerTestDefault, route_name, handler);
    
//     requestString << "GET " << "/notfound.html" << " HTTP/1.0\r\n";
//     request.ParserRequest(requestString.str());

//     // Act
//     RouteResponse *response = route.ProcessRequest(request);

//     // Assert
//     EXPECT_EQ(*expected, *response);
//     EXPECT_EQ(response->FD, nullptr);
//     EXPECT_EQ(response->Directory, nullptr);

//     // Clean
//     ServerTestDefault->ResetErroPage(404);
//     delete expected;
//     delete response;
// }

// TEST_F(RouteTest, CheckAAutoindexDirectoryListing) {
//     // Arrange
//     RouteResponse               *expected = new RouteResponse(".html", nullptr, 200);
//     HttpRequest                 request;
//     std::stringstream           requestString;
//     std::string                 route_name = "/home";
//     Route                       route(ServerTestWithAutoIndexWithoutIndex, route_name, handler);
    
//     requestString << "GET " << route_name << " HTTP/1.0\r\n";
//     request.ParserRequest(requestString.str());

//     // Act
//     RouteResponse *response = route.ProcessRequest(request);

//     // Assert
//     EXPECT_EQ(*expected, *response);
//     EXPECT_EQ(response->FD, nullptr);
//     EXPECT_NE(response->Directory, nullptr);

//     // Clean
//     free(response->Directory);
//     delete expected;
//     delete response;
// }

// TEST_F(RouteTest, CheckAAutoindexDirectoryListingNotFound) {
//     // Arrange
//     RouteResponse               *expected = new RouteResponse(".html", nullptr, 404);
//     HttpRequest                 request;
//     std::stringstream           requestString;
//     std::string                 route_name = "/DirectoryNotFound";
//     Route                       route(ServerTestWithAutoIndex, route_name, handler);
    
//     requestString << "GET " << route_name << " HTTP/1.0\r\n";
//     request.ParserRequest(requestString.str());

//     // Act
//     RouteResponse *response = route.ProcessRequest(request);

//     // Assert
//     EXPECT_EQ(*expected, *response);
//     EXPECT_NE(response->FD, nullptr);
//     EXPECT_EQ(response->Directory, nullptr);

//     // Clean
//     delete expected;
//     delete response;
// }

// TEST_F(RouteTest, CheakANotAllowDirectory) {
//     // Arrange
//     RouteResponse               *expected = new RouteResponse(".html", nullptr, 403);
//     HttpRequest                 request;
//     std::stringstream           requestString;
//     std::string                 route_name = "/notallow";
//     Route                       route(ServerTestWithAutoIndex, route_name, handler);
    
//     requestString << "GET " << route_name << " HTTP/1.0\r\n";
//     request.ParserRequest(requestString.str());

//     // Act
//     RouteResponse *response = route.ProcessRequest(request);

//     // Assert
//     EXPECT_EQ(*expected, *response);
//     EXPECT_EQ(response->FD, nullptr);
//     EXPECT_EQ(response->Directory, nullptr);

//     // Clean
//     delete expected;
//     delete response;
// }

// TEST_F(RouteTest, CheakANotAllowFile) {
//     // Arrange
//     RouteResponse               *expected = new RouteResponse(".html", nullptr, 403);
//     HttpRequest                 request;
//     std::stringstream           requestString;
//     std::string                 route_name = "/notallow.html";
//     Route                       route(ServerTestWithAutoIndex, route_name, handler);
    
//     requestString << "GET " << route_name << " HTTP/1.0\r\n";
//     request.ParserRequest(requestString.str());

//     // Act
//     RouteResponse *response = route.ProcessRequest(request);

//     // Assert
//     EXPECT_EQ(*expected, *response);
//     EXPECT_EQ(response->FD, nullptr);
//     EXPECT_EQ(response->Directory, nullptr);

//     // Clean
//     delete expected;
//     delete response;
// }


// TEST_F(RouteTest, Handler) {
//     // Arrange
//     IHandler *handler2 = new Handler();

//     EXPECT_TRUE(handler2->IsAllowToGetFile("../../build/lib/libgtest.a"));
// }

int main(int argc, char **argv) {
    // Setup
	::testing::InitGoogleTest(&argc, argv);

    // Run
	int num = RUN_ALL_TESTS();

    // Clean
    delete ServerTestDefault;
    delete ServerTestWithoutGet;
    delete ServerTestWithAutoIndex;
    delete ServerTestWithAutoIndexWithoutIndex;
    delete handler;
    return num;
}