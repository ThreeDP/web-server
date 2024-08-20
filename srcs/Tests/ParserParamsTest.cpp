# include <gtest/gtest.h>
# include <iostream>
# include "ParserParams.hpp"

class ParserParamsTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
};

TEST_F(ParserParamsTest, PassALine_ShouldBeRootParam_WhenLineContentHsARootLine) {
	// ARRANGE
	std::string		inputString = "root";

    // ACT
	EnumParams::Param result = ParserParams::IdentifiesParameter(inputString);

    // ASSERT
    EXPECT_EQ(EnumParams::_ROOT, result);
}

TEST_F(ParserParamsTest, PassALine_ShouldBeReWrite_WhenLineContentHasARewrite) {
	// ARRANGE
	std::string		inputString = "rewrite";

    // ACT
	EnumParams::Param result = ParserParams::IdentifiesParameter(inputString);

    // ASSERT
    EXPECT_EQ(EnumParams::_REWRITE, result);
}

TEST_F(ParserParamsTest, PassALine_ShouldBeServerName_WhenLineContentHasAServer_Name) {
	// ARRANGE
	std::string		inputString = "server_name";

    // ACT
	EnumParams::Param result = ParserParams::IdentifiesParameter(inputString);

    // ASSERT
    EXPECT_EQ(EnumParams::_SERVER_NAME, result);
}

TEST_F(ParserParamsTest, PassALine_ShouldBeThrowInvalidArgument_WhenLineContentIsNotSet) {
	// ARRANGE
	std::string		inputString = "ranna";

    // ACT & ASSERT
    EXPECT_THROW({
		ParserParams::IdentifiesParameter(inputString);
    }, std::invalid_argument);
}

TEST_F(ParserParamsTest, PassALine_ShouldBeListen_WhenLineContentHasListen) {
	// ARRANGE
	std::string		inputString = "listen";

    // ACT
	EnumParams::Param result = ParserParams::IdentifiesParameter(inputString);

    // ASSERT
    EXPECT_EQ(EnumParams::_LISTEN, result);
}

TEST_F(ParserParamsTest, PassALine_ShouldBeErrorPage_WhenLineContentHasErrorPage) {
	// ARRANGE
	std::string		inputString = "error_page";

    // ACT
	EnumParams::Param result = ParserParams::IdentifiesParameter(inputString);

    // ASSERT
    EXPECT_EQ(EnumParams::_ERROR_PAGE, result);
}

TEST_F(ParserParamsTest, PassALine_ShouldBeBodyLimit_WhenLineContentHasCliente_max_body_size) {
	// ARRANGE
	std::string		inputString = "client_max_body_size";

    // ACT
	EnumParams::Param result = ParserParams::IdentifiesParameter(inputString);

    // ASSERT
    EXPECT_EQ(EnumParams::_BODY_LIMIT, result);
}

TEST_F(ParserParamsTest, PassALine_ShouldBeAllow_methods_WhenLineContentHasAllowMethods) {
	// ARRANGE
	std::string		inputString = "allow_methods";

    // ACT
	EnumParams::Param result = ParserParams::IdentifiesParameter(inputString);

    // ASSERT
    EXPECT_EQ(EnumParams::_ALLOW_METHODS, result);
}

TEST_F(ParserParamsTest, PassALine_ShouldBeAutoindex_WhenLineContentHasAutoindex) {
	// ARRANGE
	std::string		inputString = "autoindex";

    // ACT
	EnumParams::Param result = ParserParams::IdentifiesParameter(inputString);

    // ASSERT
    EXPECT_EQ(EnumParams::_AUTOINDEX, result);
}

TEST_F(ParserParamsTest, PassALine_ShouldBeIndex_WhenLineContentHasIndex) {
	// ARRANGE
	std::string		inputString = "index";

    // ACT
	EnumParams::Param result = ParserParams::IdentifiesParameter(inputString);

    // ASSERT
    EXPECT_EQ(EnumParams::_INDEX, result);
}

TEST_F(ParserParamsTest, PassALine_ShouldBeAVector_WhenHasNormalPatternParam) {
	// ARRANGE
	std::string		inputString = "			  client_max_body_size 2M;";

    // ACT
	std::vector<std::string> result = ParserParams::BreakLineIntoPieces(inputString, ';');

    // ASSERT
    EXPECT_EQ(3, result.size());
    EXPECT_EQ("client_max_body_size", result[0]);
    EXPECT_EQ("2M", result[1]);
    EXPECT_EQ(";", result[2]);
}

TEST_F(ParserParamsTest, PassALine_ShouldBeAVector_WhenIsANormalPatternWithSpacesBeforeSemicolon) {
	// ARRANGE
	std::string		inputString = "			  client_max_body_size 2M   ;";

    // ACT
	std::vector<std::string> result = ParserParams::BreakLineIntoPieces(inputString, ';');

    // ASSERT
    EXPECT_EQ(3, result.size());
    EXPECT_EQ("client_max_body_size", result[0]);
    EXPECT_EQ("2M", result[1]);
    EXPECT_EQ(";", result[2]);
}

TEST_F(ParserParamsTest, PassALine_ShouldBeThrowInvalidArgument_WhenNormalPatternNotEndWithSemicolon) {
	// ARRANGE
	std::string		inputString = "			  client_max_body_size 2M   ; ";

    // ACT & ASSERT
    EXPECT_THROW({
		ParserParams::BreakLineIntoPieces(inputString, ';');
    }, std::invalid_argument);
}

TEST_F(ParserParamsTest, PassALine_ShouldBeAVector_WhenIsASectionPatternWithSpacesBeforeCurlyBrace) {
	// ARRANGE
	std::string		inputString = "			  location /home {";

    // ACT
	std::vector<std::string> result = ParserParams::BreakLineIntoPieces(inputString, '{');

    // ASSERT
    EXPECT_EQ(3, result.size());
    EXPECT_EQ("location", result[0]);
    EXPECT_EQ("/home", result[1]);
    EXPECT_EQ("{", result[2]);
}


int main(int argc, char **argv) {
    // Setup
	::testing::InitGoogleTest(&argc, argv);

    // Run
	int num = RUN_ALL_TESTS();

    // Clean
	return num;
}