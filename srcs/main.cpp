/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dapaulin <dapaulin@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/03 17:50:41 by dapaulin          #+#    #+#             */
/*   Updated: 2023/11/03 19:31:08 by dapaulin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Http.hpp"

int main() {
	Http http;
    std::vector<std::string> servers;
    servers.push_back(std::string("localhost"));
    Server *newServer = new Server(servers, 8080);
    http.SetServer("localhost", newServer);

    try {
        http.StartPollList();
        http.StartWatchSockets();
    } catch (const std::exception &e) {
        std::cout << e.what() << std::endl;
    }
} 