# include <gtest/gtest.h>
# include <iostream>
# include "ParserParams.hpp"

class ParserParamsTest : public ::testing::Test {
protected:
    void SetUp() override {
		logger = new Logger();
    }
};

ILogger		*logger;

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

TEST_F(ParserParamsTest, CallSanitizeString_ShouldBeThrow_WhenhasMoreThenOneSemicolon) {
	// ARRANGE
	std::string inputString = "         autoindex ;on;";

    // ACT & ASSERT
    EXPECT_THROW({
		ParserParams::SanitizeString(inputString);
    }, std::invalid_argument);
}

TEST_F(ParserParamsTest, CallSanitizeString_ShouldBeThrow_WhenhasOneCurlyBrakesAndOneSemicolon) {
	// ARRANGE
	std::string inputString = "         autoindex on; {";

    // ACT & ASSERT
    EXPECT_THROW({
		ParserParams::SanitizeString(inputString);
    }, std::invalid_argument);
}

TEST_F(ParserParamsTest, CallSanitizeString_ShouldBeThrow_WhenhasOneOpenCurlyBrakesAndOneCloseCurlyBrakes) {
	// ARRANGE
	std::string inputString = "         server {}";

    // ACT & ASSERT
    EXPECT_THROW({
		ParserParams::SanitizeString(inputString);
    }, std::invalid_argument);
}

TEST_F(ParserParamsTest, CallSanitizeString_ShouldBeNotThrow_WhenHasACorrectParam) {
	// ARRANGE
	std::string inputString = "         server {";

    // ACT & ASSERT
    EXPECT_NO_THROW({
		ParserParams::SanitizeString(inputString);
    });
}

TEST_F(ParserParamsTest, GetVectorParams_ShouldReturnMiddleElements_WhenVectorHasMultipleElements) {
	// ARRANGE
	std::vector<std::string> inputVector = {"first", "middle1", "middle2", "last"};

	// ACT
	std::vector<std::string> result = ParserParams::GetVectorParams(inputVector);

	// ASSERT
	std::vector<std::string> expected = {"middle1", "middle2"};
	EXPECT_EQ(result, expected);
}

TEST_F(ParserParamsTest, GetVectorParams_ShouldThrowInvalidArgument_WhenVectorHasTwoElements) {
	// ARRANGE
	std::vector<std::string> inputVector = {"first", "last"};

	// ACT && ASSERT
    EXPECT_THROW({
	    ParserParams::GetVectorParams(inputVector);
    }, std::invalid_argument);
}

TEST_F(ParserParamsTest, GetVectorParams_ShouldThrowInvalidArgument_WhenVectorHasOneElement) {
	// ARRANGE
	std::vector<std::string> inputVector = {"only"};

	// ACT && ASSERT
    EXPECT_THROW({
	    ParserParams::GetVectorParams(inputVector);
    }, std::invalid_argument);
}

TEST_F(ParserParamsTest, GetVectorParams_ShouldThrowInvalidArgument_WhenVectorIsEmpty) {
	// ARRANGE
	std::vector<std::string> inputVector = {};

	// ACT && ASSERT
    EXPECT_THROW({
	    ParserParams::GetVectorParams(inputVector);
    }, std::invalid_argument);
}

TEST_F(ParserParamsTest, GetSetParams_ShouldReturnMiddleElementsAsSet_WhenVectorHasMultipleElements) {
	// ARRANGE
	std::vector<std::string> inputVector = {"first", "middle1", "middle2", "middle3", "last"};

	// ACT
	std::set<std::string> result = ParserParams::GetSetParams(inputVector);

	// ASSERT
	std::set<std::string> expected = {"middle1", "middle2", "middle3"};
	EXPECT_EQ(result, expected);
}

TEST_F(ParserParamsTest, GetSetParams_ShouldThrowInvalidArgument_WhenVectorHasTwoElements) {
	// ARRANGE
	std::vector<std::string> inputVector = {"first", "last"};

	// ACT && ASSERT
    EXPECT_THROW({
	    ParserParams::GetSetParams(inputVector);
    }, std::invalid_argument);
}

TEST_F(ParserParamsTest, GetSetParams_ShouldThrowInvalidArgument_WhenVectorHasOneElement) {
	// ARRANGE
	std::vector<std::string> inputVector = {"only"};

	// ACT && ASSERT
    EXPECT_THROW({
	    ParserParams::GetSetParams(inputVector);
    }, std::invalid_argument);
}

TEST_F(ParserParamsTest, GetSetParams_ShouldThrowInvalidArgument_WhenVectorIsEmpty) {
	// ARRANGE
	std::vector<std::string> inputVector = {};

	// ACT && ASSERT
    EXPECT_THROW({
        ParserParams::GetSetParams(inputVector);
    }, std::invalid_argument);
}

TEST_F(ParserParamsTest, GetSetParams_ShouldReturnUniqueMiddleElements_WhenVectorHasDuplicateMiddleElements) {
	// ARRANGE
	std::vector<std::string> inputVector = {"first", "middle1", "middle2", "middle1", "last"};

	// ACT
	std::set<std::string> result = ParserParams::GetSetParams(inputVector);

	// ASSERT
	std::set<std::string> expected = {"middle1", "middle2"};
	EXPECT_EQ(result, expected);
}

TEST_F(ParserParamsTest, GetPairParams_ShouldReturnPair_WhenVectorHasCorrectNumberOfParams) {
	// ARRANGE
	std::vector<std::string> inputVector = {"rewrite", "param1", "param2", ";"};

	// ACT
	std::pair<std::string, std::string> result = ParserParams::GetPairParams(inputVector);

	// ASSERT
	EXPECT_EQ(result.first, "param1");
	EXPECT_EQ(result.second, "param2");
}

TEST_F(ParserParamsTest, GetPairParams_ShouldThrowInvalidArgument_WhenVectorHasLessThanFourElements) {
	// ARRANGE
	std::vector<std::string> inputVector = {"command", "param1", ";"};

	// ACT & ASSERT
	EXPECT_THROW({
		ParserParams::GetPairParams(inputVector);
	}, std::invalid_argument);
}

TEST_F(ParserParamsTest, GetPairParams_ShouldThrowInvalidArgument_WhenVectorHasMoreThanFourElements) {
	// ARRANGE
	std::vector<std::string> inputVector = {"command", "param1", "param2", ";", "extraParam"};

	// ACT & ASSERT
	EXPECT_THROW({
		ParserParams::GetPairParams(inputVector);
	}, std::invalid_argument);
}

TEST_F(ParserParamsTest, GetPairParams_ShouldThrowInvalidArgument_WhenVectorIsEmpty) {
	// ARRANGE
	std::vector<std::string> inputVector = {};

	// ACT & ASSERT
	EXPECT_THROW({
		ParserParams::GetPairParams(inputVector);
	}, std::invalid_argument);
}

TEST_F(ParserParamsTest, GetPairParams_ShouldThrowInvalidArgument_WhenVectorHasOnlyOneElement) {
	// ARRANGE
	std::vector<std::string> inputVector = {"command"};

	// ACT & ASSERT
	EXPECT_THROW({
		ParserParams::GetPairParams(inputVector);
	}, std::invalid_argument);
}

// TEST_F(ParserParamsTest, GetPairParams_ShouldThrowInvalidArgument_WhenVectorHasOnlyOneElement) {
// 	// ARRANGE
// 	Parser()
// 	std::vector<std::string> inputVector = {"command"};

// 	// ACT & ASSERT
// 	EXPECT_THROW({
// 		ParserParams::GetPairParams(inputVector);
// 	}, std::invalid_argument);
// }


int main(int argc, char **argv) {
    // Setup
	::testing::InitGoogleTest(&argc, argv);

    // Run
	int num = RUN_ALL_TESTS();

    // Clean
	// delete logger;
	return num;
}