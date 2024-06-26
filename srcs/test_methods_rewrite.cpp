#include <iostream>
#include <fstream>
#include <sstream>
#include "./Parser/Parser.hpp"
#include "./Server/Http.hpp"
#include "./Server/Server.hpp"

void runParserTests() {
    Http http;
    try {
        Parser parser("./file_config_test/error_rewrite_empty.conf");
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
            std::cout << std::endl;
            std::cout << "path: " << rootRoute->GetRedirectPath() << std::endl;
        }

    } catch (const std::exception &e) {
        std::cerr << "Erro: " << e.what() << std::endl;
    }
}

void runError(){
    Http http;
    try {
        Parser parser("./file_config_test/test_config1.conf");
        parser.ParserServer(http);

        Server *server = http.GetServer("teste");
    } catch (const std::exception &e) {
        std::cout << "\033[1m" << "\033[92m" << "==============  Sucesso Test1 =============" << std::endl;
    }

}

void runTest2(){
    Http http;

    try{
        Parser parser("./file_config_test/test_error2.conf");
        parser.ParserServer(http);
        Server *server = http.GetServer("teste");

        Route *rootRoute = server->routes["/"];
        Route *imgRoute = server->routes["/img"];

        if (rootRoute->GetRedirectPath() != "/new_path" || imgRoute->GetRedirectPath() != "/images"){
            std::cout << "\033[1m" << "\033[91m" << "==============  Error Test2 =============" << std::endl;
        } else {
            std::cout << "\033[1m" << "\033[92m" << "==============  Sucesso Test2 =============" << std::endl;
        }

    }catch (const std::exception &e){
        std::cout << "\033[1m" << "\033[91m" << "==============  Error Test2 =============" << std::endl;
    }
}

void runTest3(){
    Http http;

    try{
        Parser parser("./file_config_test/test_error3.conf");
        parser.ParserServer(http);
        Server *server = http.GetServer("teste");

        Route *rootRoute = server->routes["/"];
        Route *imgRoute = server->routes["/img"];
        std::cout << "\033[1m" << "\033[92m" << "==============  Sucesso Test3 =============" << std::endl;

    }catch (const std::exception &e){
        std::cout << "\033[1m" << "\033[91m" << "==============  Error Test3 =============" << std::endl;
    }
}

void runTest4(){
    Http http;

    try{
        Parser parser("./file_config_test/test_error4.conf");
        parser.ParserServer(http);
        Server *server = http.GetServer("teste");

        Route *rootRoute = server->routes["/"];
        Route *imgRoute = server->routes["/img"];

    }catch (const std::exception &e){
        std::cout << "\033[1m" << "\033[92m" << "==============  Sucesso Test4 =============" << std::endl;
    }
}

void runTest5(){
    Http http;

    try{
        Parser parser("./file_config_test/test_error5.conf");
        parser.ParserServer(http);
        Server *server = http.GetServer("teste");

    

    }catch (const std::exception &e){
        std::cout << "\033[1m" << "\033[92m" << "==============  Sucesso Test5 =============" << std::endl;
    }
}

void runTest6() {
    Http http;

    try {
        Parser parser("./file_config_test/test_error6.conf");
        parser.ParserServer(http);
        Server *server = http.GetServer("teste");

        // Print default error pages
        std::cout << "Server default error page: " << std::endl;
        std::map<int, std::string> default_error_pages = server->GetDefaultErrorPage();
        std::cout << "Default error pages:" << std::endl;
        for (std::map<int, std::string>::iterator it = default_error_pages.begin(); it != default_error_pages.end(); ++it) {
            std::cout << "Error " << it->first << ": " << it->second << std::endl;
        }

        Route *rootRoute = server->routes["/"];
        Route *imgRoute = server->routes["/img"];
        Route *staticRoute = server->routes["/static"];
        Route *homeRoute = server->routes["/home"];



        // route root
        std::cout << "root error_page: " << std::endl;
        std::map<int, std::string> root_error_pages = rootRoute->GetErrorPage();
        for (std::map<int, std::string>::iterator it = root_error_pages.begin(); it != root_error_pages.end(); ++it) {
            std::cout << "Error " << it->first << ": " << it->second << std::endl;
        }

        // route img
        std::cout << "\n\nimg error_page: " << std::endl;
        std::map<int, std::string> img_error_pages = imgRoute->GetErrorPage();
        for (std::map<int, std::string>::iterator it = img_error_pages.begin(); it != img_error_pages.end(); ++it) {
            std::cout << "Error " << it->first << ": " << it->second << std::endl;
        }

        // route static
        std::cout << "\n\nstatic error_page: " << std::endl;
        std::map<int, std::string> static_error_pages = staticRoute->GetErrorPage();
        for (std::map<int, std::string>::iterator it = static_error_pages.begin(); it != static_error_pages.end(); ++it) {
            std::cout << "Error " << it->first << ": " << it->second << std::endl;
        }


        // route home
        std::cout << "\n\nhome error_page: " << std::endl;
        std::map<int, std::string> home_error_pages = homeRoute->GetErrorPage();
        for (std::map<int, std::string>::iterator it2 = home_error_pages.begin(); it2 != home_error_pages.end(); ++it2) {
            std::cout << "Error " << it2->first << ": " << it2->second << std::endl;
        }
        std::cout << "\033[1m" << "\033[92m" << "==============  Sucesso Test6 =============" << std::endl;
    } catch (const std::exception &e) {
        std::cout << "\033[1m" << "\033[91m" << "==============  Error Test6 =============" << std::endl;
        std::cerr << e.what() << std::endl;
    }
}



int main() {
    // runParserTests();
    // runError();
    // runTest2();
    // runTest3();
    // runTest4();
    // runTest5();
   runTest6();
    return 0;
}
