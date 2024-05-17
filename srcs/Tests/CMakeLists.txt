cmake_minimum_required(VERSION 3.14)
cmake_policy(SET CMP0079 NEW)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(Name WebServerTests)
set(Sources parser_test.cpp)
project(${Name})

enable_testing()
find_package(GTest REQUIRED)
include(GoogleTest)

add_executable(
	parser_test
  	${Sources}
)

target_link_libraries(
	parser_test
	WebServer
  	GTest::gtest_main
)

gtest_discover_tests(parser_test)
