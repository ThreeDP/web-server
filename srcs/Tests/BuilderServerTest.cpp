# include <gtest/gtest.h>
# include <iostream>
# include "BuilderServer.hpp"
# include <climits>

IBuilderServer  *builder;
IHandler        *handler;
ILogger         *logger;

class BuilderServerTest : public ::testing::Test {
protected:
    void SetUp() override {
        handler = new Handler();
        logger = new Logger();
        builder = new BuilderServer(handler, logger);
    }
};

TEST_F(BuilderServerTest, BuilderWithAutoIndex_ShouldEnableAutoIndex_WhenPassTrue) {
    // ACT
    builder->SetupServer()
        .WithAutoIndex(true);
    IServer *result = builder->GetResult();

    // ASSERT
    EXPECT_EQ(result->GetAutoIndex(), true);
}

TEST_F(BuilderServerTest, BuilderWithBodyLimit_ShouldConfigureExtremeBodyLimit_WhenHighAndLowLimitsProvided) {
    // Arrange
    int HighBodyLimit = INT_MAX;
    int LowBodyLimit = 1;

    // ACT
    builder->SetupServer()
        .WithBodyLimit(HighBodyLimit);
    IServer *resultHigh = builder->GetResult();

    builder->SetupServer()
        .WithBodyLimit(LowBodyLimit);
    IServer *resultLow = builder->GetResult();

    // ASSERT
    EXPECT_EQ(resultHigh->GetBodyLimit(), HighBodyLimit);
    EXPECT_EQ(resultLow->GetBodyLimit(), LowBodyLimit);
}

TEST_F(BuilderServerTest, BuilderWithErrorPages_Should_ConfigurePartialErrorPages_WhenOnlySomeErrorPagesProvided) {
    // Arrange
    std::set<HttpStatusCode::Code> InternalServerErroPage = {HttpStatusCode::_INTERNAL_SERVER_ERROR};
    std::string InternalPath = "50x.html";

    // ACT
    builder->SetupServer()
        .WithErrorPages(InternalServerErroPage, InternalPath);
    IServer *result = builder->GetResult();

    // ASSERT
    std::map<HttpStatusCode::Code, std::string> errorPages = result->GetErrorPages();
    EXPECT_EQ(errorPages[HttpStatusCode::_INTERNAL_SERVER_ERROR], "50x.html");
    EXPECT_TRUE(errorPages.find(HttpStatusCode::_NOT_FOUND) == errorPages.end()); // NotFoundPage não foi configurado
}

TEST_F(BuilderServerTest, BuilderWithAllowMethods_Should_ConfigureAllowedMethods_WhenValidMethodsProvided) {
    // Arrange
    std::set<std::string> Methods = {"GET", "POST", "PUT"};

    // ACT
    builder->SetupServer()
        .WithAllowMethods(Methods);
    IServer *result = builder->GetResult();

    // ASSERT
    EXPECT_EQ(result->GetAllowMethods(), Methods);
}

TEST_F(BuilderServerTest, BuilderWithRedict_Should_ConfigureEmptyRedirectPath_WhenEmptyRedirectPathProvided) {
    // Arrange
    std::pair<std::string, std::string> EmptyRedirect = {"", ""};

    // ACT
    builder->SetupServer()
        .WithRedirectPath(EmptyRedirect);
    IServer *result = builder->GetResult();

    // ASSERT
    EXPECT_EQ(result->GetRedirectPath(""), "");
}

TEST_F(BuilderServerTest, builderWithPort_Should_ThrowException_WhenInvalidPortProvided) {
    // Arrange
    std::vector<std::string> Hosts = {"localhost"};
    std::string InvalidMaxPort = "99999";  // Porta fora do intervalo válido
    std::string InvalidMinPort = "-1";  // Porta fora do intervalo válido

    // ACT & ASSERT
    EXPECT_THROW({
        builder->SetupServer()
            .WithHosts(Hosts)
            .WithPort(InvalidMaxPort);
        builder->GetResult();
    }, std::invalid_argument);

    EXPECT_THROW({
        builder->SetupServer()
            .WithHosts(Hosts)
            .WithPort(InvalidMinPort);
        builder->GetResult();
    }, std::invalid_argument);
}

TEST_F(BuilderServerTest, BuilderWithHosts_Should_ConfigureWithPatternHosts_WhenNoHostsProvided) {
    // Arrange
    std::vector<std::string> PatternHosts = {"localhost"};
    std::vector<std::string> Hosts = {};
    std::string Port = "8081";

    // ACT
    builder->SetupServer()
        .WithHosts(Hosts)
        .WithPort(Port);
    IServer *result = builder->GetResult();

    // ASSERT
    EXPECT_EQ(result->GetHosts(), PatternHosts);
}


TEST_F(BuilderServerTest, TestOne) {
    // Arrange
    std::vector<std::string>                   Hosts = {"localhost"};
    std::string                             Port = "8081";
    std::set<std::string>                   Methods = {"GET", "POST"};
    std::set<HttpStatusCode::Code>          InternalServerErroPage = {HttpStatusCode::_INTERNAL_SERVER_ERROR};
    std::string                             InternalPath = "50x.html";
    std::set<HttpStatusCode::Code>          NotFoundPage = {HttpStatusCode::_NOT_FOUND};
    std::string                             NotFoundPath = "404.html";
    int                                     BodyLimit = 2048;
    std::pair<std::string, std::string>     Redirect = {"/rota1", "rota2"};
    std::string                             Root = "/home";
    bool                                    AutoIndex = false;
    
    // ACT
    builder->SetupServer()
        .WithHosts(Hosts)
        .WithPort(Port)
        .WithAllowMethods(Methods)
        .WithErrorPages(InternalServerErroPage, InternalPath)
        .WithErrorPages(NotFoundPage, NotFoundPath)
        .WithBodyLimit(BodyLimit)
        .WithRedirectPath(Redirect)
        .WithRootDirectory(Root);
    IServer *result = builder->GetResult();

    // ASSERT
    EXPECT_EQ(2048, result->GetBodyLimit());
    EXPECT_EQ("rota2", result->GetRedirectPath(Redirect.first));
    EXPECT_EQ("/home", result->GetRootDirectory());
    EXPECT_EQ(false, result->GetAutoIndex());
    EXPECT_EQ(Hosts, result->GetHosts());
    EXPECT_EQ("8081", result->GetPort());
    EXPECT_EQ(Methods, result->GetAllowMethods());
    std::map<HttpStatusCode::Code, std::string> errorPages = result->GetErrorPages();
    EXPECT_EQ("404.html", errorPages[HttpStatusCode::_NOT_FOUND]);
    EXPECT_EQ("50x.html", errorPages[HttpStatusCode::_INTERNAL_SERVER_ERROR]);
}

int main(int argc, char **argv) {
    // Setup
	::testing::InitGoogleTest(&argc, argv);

    // Run
	int num = RUN_ALL_TESTS();

    // Clean
    return num;
}