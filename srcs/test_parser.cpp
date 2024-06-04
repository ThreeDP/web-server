#include <iostream>
#include <fstream>
#include <sstream>
#include "./Parser/Parser.hpp"
#include "./Server/Http.hpp"
#include "./Server/Server.hpp"

void runParserTests() {
    std::ofstream configFile("test_config.conf");
    configFile << 
    "server {\n"
    "    server_name localhost;\n"
    "    location / {\n"
    "        allow_methods GET POST;\n"
    "        rewrite /test.com /test.com/post;\n"
    "    }\n"
    "    location /static {\n"
    "        allow_methods GET;\n"
    "    }\n"
    "}\n";
    configFile.close();
    Http http;
    Parser parser("test_config.conf");

    try {
        parser.ParserServer(http);

        Server *server = http.GetServer("localhost");
        if (server == NULL) {
            std::cerr << "Erro: 'localhost' não encontrado" << std::endl;
        } else {
            std::cout << "'localhost' encontrado com sucesso" << std::endl;
            std::cout << "Nomes do servidor: " << std::endl;
            for (std::vector<std::string>::iterator it = server->server_names.begin(); it != server->server_names.end(); ++it) {
                std::cout << *it << " ";
            }
            std::cout << std::endl;
        }

        Route *rootRoute = server->routes["/"];
        Route *staticRoute = server->routes["/static"];

        if (rootRoute == NULL) {
            std::cerr << "Erro: Rota '/' não encontrada" << std::endl;
        } else {
            std::cout << "Rota '/' encontrada com sucesso" << std::endl;
            std::cout << "Métodos permitidos: ";
            for (std::vector<std::string>::iterator it = rootRoute->GetAllowMethods().begin(); it != rootRoute->GetAllowMethods().end(); ++it) {
                std::cout << *it << " ";
            }
            std::cout << std::endl;
            std::cout << "Redirecionamentos: ";
            std::map<std::string, std::string> redirectPaths = rootRoute->GetRedirectPath();
            for (std::map<std::string, std::string>::iterator it = redirectPaths.begin(); it != redirectPaths.end(); ++it) {
                std::cout << it->first << " -> " << it->second << " ";
            }
            std::cout << std::endl;
        }

        if (staticRoute == NULL) {
            std::cerr << "Erro: Rota '/static' não encontrada" << std::endl;
        } else {
            std::cout << "Rota '/static' encontrada com sucesso" << std::endl;
            std::cout << "Métodos permitidos: ";
            for (std::vector<std::string>::iterator it = staticRoute->GetAllowMethods().begin(); it != staticRoute->GetAllowMethods().end(); ++it) {
                std::cout << *it << " ";
            }
            std::cout << std::endl;
        }

    } catch (const std::exception &e) {
        std::cerr << "Erro: " << e.what() << std::endl;
    }
}

int main() {
    runParserTests();
    return 0;
}
