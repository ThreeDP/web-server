/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dapaulin <dapaulin@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/03 17:50:41 by dapaulin          #+#    #+#             */
/*   Updated: 2024/08/01 20:00:03 by dapaulin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Http.hpp"
# include "BuilderServer.hpp"

int main() {
    std::set<std::string> methods;
    methods.insert("GET");
    methods.insert("INFO");
    std::set<HttpStatusCode::Code> statusCode;
    statusCode.insert(HttpStatusCode::_NOT_FOUND);
    std::vector<std::string> indexes;
    indexes.push_back("index.html");
    indexes.push_back("index.htm");


	Http http;
    IHandler *handler = new Handler();
    IBuilderServer *b = new BuilderServer(handler);
    IServer *newServer = b->SetupServer()
                            .WithAllowMethods(methods)
                            .WithErrorPages(statusCode, "404.html")
                            .WithPort("8081")
                            .WithRootDirectory("../home")
                            // .WithPageIndexes(indexes)
                            .GetResult();
    //Server *newServer2 = new Server("localhost2", 8082, ".");
    http.SetServer("localhost", newServer);
    //http.SetServer("localhost2", newServer2);
    IBuilderRoute   *builder = new BuilderRoute(newServer, handler);
    newServer->SetRoute("/", builder->SetupRoute("/")
                                    .GetResult());
    newServer->SetRoute("/app", builder->SetupRoute("/app")
                                    .GetResult());
    // newServer->_routes["/static/imagens"] = builder->SetupRoute("/static/imagens")
    //                                         .GetResult();
    // newServer->_routes["/ranna-site"] = builder->SetupRoute("/ranna-site")
    //                                     .GetResult();
    // newServer2->routes["/static"] = new Route(newServer2, "/static");
    try {
        http.StartPollList();
        http.StartWatchSockets();
    } catch (const std::exception &e) {
        std::cout << e.what() << std::endl;
    }
} 