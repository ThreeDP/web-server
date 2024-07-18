cmake_minimum_required(VERSION 3.14)
cmake_policy(SET CMP0079 NEW)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(Name WebServerTests)
set(ParserSources parser_test.cpp)
set(ResponseSources ResponseTest.cpp)
project(${Name})

enable_testing()
find_package(GTest REQUIRED)
include(GoogleTest)

add_executable(
	ParserTest
  	${ParserSources}
)

add_executable(
	ResponseTest
  	${ResponseSources}
)

target_link_libraries(
	ParserTest
	WebServer
  	GTest::gtest_main
)

target_link_libraries(
	ResponseTest
	WebServer
  	GTest::gtest_main
)

gtest_discover_tests(ParserTest)
gtest_discover_tests(ResponseTest)
