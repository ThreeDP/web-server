/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dapaulin <dapaulin@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/03 17:50:41 by dapaulin          #+#    #+#             */
/*   Updated: 2024/06/01 20:00:45 by dapaulin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Http.hpp"

int main() {
	Http http;
    Server *newServer = new Server("localhost");
    Server *newServer2 = new Server("localhost2", 8082, ".");
    http.SetServer("localhost", newServer);
    http.SetServer("localhost2", newServer2);
    newServer->routes["/"] = new Route(newServer, "/");
    newServer->routes["/app"] = new Route(newServer, "/app");
    newServer->routes["/static/imagens"] = new Route(newServer, "/static/imagens");
    newServer->routes["/ranna-site"] = new Route(newServer, "/ranna-site");
    newServer2->routes["/static"] = new Route(newServer2, "/static");

    try {
        http.StartPollList();
        http.StartWatchSockets();
    } catch (const std::exception &e) {
        std::cout << e.what() << std::endl;
    }
} 