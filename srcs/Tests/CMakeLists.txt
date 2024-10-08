cmake_minimum_required(VERSION 3.14)
cmake_policy(SET CMP0079 NEW)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(GTEST_INCLUDE_DIR "/workspace/web-server/srcs/googletest/googletest/include")

set(Name WebServerTests)
set(BuilderServer BuilderServerTest.cpp)
set(ParserParams ParserParamsTest.cpp)
project(${Name})

enable_testing()
find_package(GTest REQUIRED)
include(GoogleTest)

add_executable(
	BuilderServerTest
	${BuilderServer}
)

add_executable(
	ParserParamsTest
	${ParserParams}
)

target_link_libraries(
	BuilderServerTest
	WebServer
  	GTest::gtest_main
)

target_link_libraries(
	ParserParamsTest
	WebServer
  	GTest::gtest_main
)

gtest_discover_tests(BuilderServerTest)
gtest_discover_tests(ParserParamsTest)
