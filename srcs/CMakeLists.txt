cmake_minimum_required(VERSION 3.14)
set(Name WebServer)
project(${Name})
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

enable_testing()

set(INCLUDE_DIR 
    "${CMAKE_CURRENT_SOURCE_DIR}/Parser"
    "${CMAKE_CURRENT_SOURCE_DIR}/Server"
)

set(HEADERS
    Parser/Parser.hpp
    Server/Server.hpp
    Server/Route.hpp
    Server/Http.hpp
)

# Source files
set(SOURCE_FILES
    Parser/Parser.cpp
    Server/Server.cpp
    Server/Route.cpp
    Server/Http.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})

target_include_directories(${Name} PUBLIC ${INCLUDE_DIR})

add_subdirectory(googletest)
add_subdirectory(Tests)