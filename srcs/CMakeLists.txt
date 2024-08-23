cmake_minimum_required(VERSION 3.14)
set(Name WebServer)
project(${Name})
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

enable_testing()

set(INCLUDE_DIR 
    "${CMAKE_CURRENT_SOURCE_DIR}/Parser"
    "${CMAKE_CURRENT_SOURCE_DIR}/Http"
    "${CMAKE_CURRENT_SOURCE_DIR}/Http/Request"
    "${CMAKE_CURRENT_SOURCE_DIR}/Http/Response"
    "${CMAKE_CURRENT_SOURCE_DIR}/Http/Route"
    "${CMAKE_CURRENT_SOURCE_DIR}/Http/Server"
    "${CMAKE_CURRENT_SOURCE_DIR}/Logging"
    "${CMAKE_CURRENT_SOURCE_DIR}/Utils"
    "${CMAKE_CURRENT_SOURCE_DIR}/HandlerFileSystem"
)

set(HEADERS
    Utils/Utils.hpp
    HandlerFileSystem/IHandler.hpp
    HandlerFileSystem/Handler.hpp
    Parser/ParserParams.hpp
    Logging/ILogger.hpp
    Logging/Logger.hpp
    Http/Server/IBuilderServer.hpp
    Http/Server/BuilderServer.hpp
    Http/Server/IServer.hpp
    Http/Server/Server.hpp
    Http/Route/IBuilderRoute.hpp
    Http/Route/BuilderRoute.hpp
    Http/Route/IRoute.hpp
    Http/Route/Route.hpp
    Http/Response/IBuilderResponse.hpp
    Http/Response/BuilderResponse.hpp
    Http/Response/IHttpResponse.hpp
    Http/Response/HttpResponse.hpp
    Http/Request/HttpRequest.hpp
    Http/EHttpStatusCode.hpp
    Http/define.hpp
    Http/Http.hpp
)

# Source files
set(SOURCE_FILES
    Utils/Utils.cpp
    HandlerFileSystem/Handler.cpp
    Http/Server/BuilderServer.cpp
    Http/Server/Server.cpp
    Http/Route/BuilderRoute.cpp
    Http/Route/Route.cpp
    Http/Response/BuilderResponse.cpp
    Http/Response/HttpResponse.cpp
    Http/Request/HttpRequest.cpp
    Http/Http.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})

target_include_directories(${Name} PUBLIC ${INCLUDE_DIR})

add_subdirectory(googletest)
add_subdirectory(Tests)