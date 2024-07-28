/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dapaulin <dapaulin@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/03 17:50:41 by dapaulin          #+#    #+#             */
/*   Updated: 2024/07/27 19:19:25 by dapaulin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Http.hpp"
# include "Handler.hpp"

int main() {
	Http http;
    Server *newServer = new Server("localhost");
    //Server *newServer2 = new Server("localhost2", 8082, ".");
    http.SetServer("localhost", newServer);
    //http.SetServer("localhost2", newServer2);
    IHandler *handler = new Handler();
    newServer->routes["/"] = new Route(newServer, "/", handler);
    newServer->routes["/app"] = new Route(newServer, "/app", handler);
    newServer->routes["/static/imagens"] = new Route(newServer, "/static/imagens", handler);
    newServer->routes["/ranna-site"] = new Route(newServer, "/ranna-site", handler);
    // newServer2->routes["/static"] = new Route(newServer2, "/static");

    try {
        http.StartPollList();
        http.StartWatchSockets();
    } catch (const std::exception &e) {
        std::cout << e.what() << std::endl;
    }
} 