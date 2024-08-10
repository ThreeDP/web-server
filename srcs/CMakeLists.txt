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
    "${CMAKE_CURRENT_SOURCE_DIR}/Server/Response"
    "${CMAKE_CURRENT_SOURCE_DIR}/Utils"
    "${CMAKE_CURRENT_SOURCE_DIR}/HandlerFileSystem"
)

set(HEADERS
    Parser/Parser.hpp
    Server/Server.hpp
    Server/IServer.hpp
    Server/CommonParameters.hpp
    Server/Route.hpp
    Server/Http.hpp
    Server/HttpRequest.hpp
    Server/Response/EHttpStatusCode.hpp
    Server/Response/IHttpResponse.hpp
    Server/Response/HttpResponse.hpp
    Server/IBuilderResponse.hpp
    Server/BuilderResponse.hpp
    Utils/Utils.hpp
    HandlerFileSystem/IHandler.hpp
    HandlerFileSystem/Handler.hpp
)

# Source files
set(SOURCE_FILES
    Parser/Parser.cpp
    Server/Server.cpp
    Server/CommonParameters.cpp
    Server/Route.cpp
    Server/Http.cpp
    Server/HttpRequest.cpp
    Server/Response/HttpResponse.cpp
    Utils/Utils.cpp
    HandlerFileSystem/Handler.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})

target_include_directories(${Name} PUBLIC ${INCLUDE_DIR})

add_subdirectory(googletest)
add_subdirectory(Tests)