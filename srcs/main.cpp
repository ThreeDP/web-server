/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dapaulin <dapaulin@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/03 17:50:41 by dapaulin          #+#    #+#             */
/*   Updated: 2024/06/01 14:05:48 by dapaulin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Http.hpp"

int main() {
	Http http;
    Server *newServer = new Server("localhost");
    http.SetServer("localhost", newServer);
    newServer->routes["/"] = new Route(newServer, "/");
    newServer->routes["/app"] = new Route(newServer, "/app");
    newServer->routes["/static/imagens"] = new Route(newServer, "/static/imagens");

    //try {
        http.StartPollList();
        http.StartWatchSockets();
    //} catch (const std::exception &e) {
      //  std::cout << e.what() << std::endl;
    //}
} 