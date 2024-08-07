# include <gtest/gtest.h>
# include <iostream>
# include "Utils.hpp"


class UtilsTest : public ::testing::Test {
protected:
    void SetUp() override {
        
    }
};

TEST_F(UtilsTest, SanitezePathWhenHasOneSlashInTheMiddle) {
    // Arrange
	std::string root = "/app";
	std::string path = "/index.html";
	std::string expected = "/app/index.html";

	// ACT
	std::string result = Utils::SanitizePath(root, path);

	// Assert
    EXPECT_EQ(expected, result);
}

TEST_F(UtilsTest, SanitezePathWhenHasDoubleSlashInTheMiddle) {
    // Arrange
	std::string root = "/app/";
	std::string path = "/index.html";
	std::string expected = "/app/index.html";

	// ACT
	std::string result = Utils::SanitizePath(root, path);

	// Assert
    EXPECT_EQ(expected, result);
}

TEST_F(UtilsTest, SanitezePathWhenHasNoSlashInTheMiddle) {
    // Arrange
	std::string root = "/app";
	std::string path = "index.html";
	std::string expected = "/app/index.html";

	// ACT
	std::string result = Utils::SanitizePath(root, path);

	// Assert
    EXPECT_EQ(expected, result);
}

int main(int argc, char **argv) {
    // Setup
	::testing::InitGoogleTest(&argc, argv);

    // Run
	int num = RUN_ALL_TESTS();

    // Clean
    return num;
}