#include <iostream>
#include <fstream>
#include <sstream>
#include "./Parser/Parser.hpp"
#include "./Server/Http.hpp"
#include "./Server/Server.hpp"

void runParserTests() {
    Http http;
    try {
        Parser parser("./file_config_test/test_config.conf");
        parser.ParserServer(http);

        Server *server = http.GetServer("teste");
        if (server == NULL) {
            std::cerr << "Erro: 'localhost' não encontrado" << std::endl;
        } else {
            std::cout << "'localhost' encontrado com sucesso" << std::endl;
            std::cout << "Nomes do servidor: ";
            std::vector<std::string> server_names = server->GetServerNames();
            for (std::vector<std::string>::iterator it = server_names.begin(); it != server_names.end(); ++it) {
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
            std::set<std::string> *methods = rootRoute->GetAllowMethods();
           
            for (std::set<std::string>::iterator it = rootRoute->GetAllowMethods()->begin(); it != rootRoute->GetAllowMethods()->end(); ++it) {
                std::cout << *it << " ";
            }
            std::cout << "path: " << rootRoute->GetRedirectPath() << std::endl;
        }

    } catch (const std::exception &e) {
        std::cerr << "Erro: " << e.what() << std::endl;
    }
}

void runError(){
    Http http;
    try {
        Parser parser("./file_config_test/error_rewrite_empty.conf");
        parser.ParserServer(http);

        Server *server = http.GetServer("teste");
        // Route *rootRoute = server->routes["/img"];

        // if (rootRoute->GetRedirectPath().empty()){
        //     std::cout << "failed test Error_rewrite_empty" << std::endl;
        // }
    } catch (const std::exception &e) {
        std::cout << "Sucesso Test1" << std::endl;
    }

    //  try {
    //     Parser parser("./file_config_test/failed test_config1.conf");
    //     parser.ParserServer(http);

    //     Server *server = http.GetServer("teste");
    //     Route *rootRoute = server->routes["/img"];

    //     if (rootRoute->GetRedirectPath() == "/images"){
    //         std::cout << "Sucesso Test 2" << std::endl;
    //     }else{
    //         std::cout << "failed test_config1.conf" << std::endl;
    //     }
    // } catch (const std::exception &e) {
    //     std::cout << "failed test_config1.conf" << std::endl;
    // }

    // try {
    //     Parser parser("./file_config_test/test_config2.conf.conf");
    //     parser.ParserServer(http);

    //     Server *server = http.GetServer("teste");
    //     Route *rootRoute = server->routes["/img"];

    //     if (rootRoute->GetRedirectPath() == "/images"){
    //         std::cout << "Sucesso Test 2" << std::endl;
    //     }else{
    //         std::cout << "failed test_config1.conf" << std::endl;
    //     }
    // } catch (const std::exception &e) {
    //     std::cout << "failed test_config2.conf" << std::endl;
    // }
}

int main() {
    //runParserTests();
    runError();
    return 0;
}

// #include "./Parser/Parser.hpp"

// int main()
// {
//     std::vector<Server>     servers;
//     Http http;
// 	char path_file[] = "./Parser/test_metodos/nginx.conf";

//     try {
// 	    Parser obj(path_file);
//         obj.ParserServer(http);

        
        
//     } catch (const std::exception &e) {
//         std::cout << e.what() << std::endl;
//         return 1;
//     }

// 	return(0);
// }
