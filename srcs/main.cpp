/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dapaulin <dapaulin@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/03 17:50:41 by dapaulin          #+#    #+#             */
/*   Updated: 2024/08/23 22:37:55 by dapaulin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Http.hpp"
# include "Parser.hpp"
# include "BuilderServer.hpp"

int main(int ac, char *av[]) {
    // std::set<std::string> methods;
    // methods.insert("GET");
    // methods.insert("INFO");
    // std::set<HttpStatusCode::Code> statusCode;
    // statusCode.insert(HttpStatusCode::_NOT_FOUND);
    // std::vector<std::string> indexes;
    // indexes.push_back("index.html");
    // indexes.push_back("index.htm");


    // IHandler *handler = new Handler();
    // ILogger *logger = new Logger();
	// Http http(logger);
    // IBuilderServer *b = new BuilderServer(handler, logger);
    // IServer *newServer = b->SetupServer()
    //                         .WithAllowMethods(methods)
    //                         .WithAutoIndex(true)
    //                         .WithErrorPages(statusCode, "404.html")
    //                         .WithPort("8081")
    //                         .WithRootDirectory("../home")
    //                         // .WithPageIndexes(indexes)
    //                         .GetResult();
    // //Server *newServer2 = new Server("localhost2", 8082, ".");
    // http.SetServer("localhost", newServer);
    // //http.SetServer("localhost2", newServer2);
    // IBuilderRoute   *builder = new BuilderRoute(logger, newServer, handler);
    // newServer->SetRoute("/", builder->SetupRoute("/")
    //                             .WithPageIndexes(indexes)
    //                                 .GetResult());
    // newServer->SetRoute("/app", builder->SetupRoute("/app")
    //                                 .GetResult());
    // newServer->SetRoute("/static/imagens", builder->SetupRoute("/static/imagens")
    //                                         .WithPageIndexes(indexes)
    //                                         .GetResult());
    // newServer->SetRoute("/ranna-site", builder->SetupRoute("/ranna-site")
    //                                     .WithPageIndexes(indexes)
    //                                     .GetResult());

    ILogger         *logger = new Logger();
    IHandler        *handler = new Handler();
    Http            http(logger);
    IBuilderServer  *builder = new BuilderServer(handler, logger);
    Parser          parser(logger, handler, builder);
    std::string     fileName = "./config/ravy.conf";
    if (ac > 1) {
        fileName = av[1];
    }
    try {
        std::cout << logger->Log(&Logger::LogInformation, "Starting Parser.") << std::endl;
        parser.ConfigHttp(http, fileName);
        std::cout << logger->Log(&Logger::LogInformation, "Finished Parser.") << std::endl;
        http.StartPollList();
        http.StartWatchSockets();
    } catch (const std::exception &e) {
        std::cout << e.what() << std::endl;
    }
} 