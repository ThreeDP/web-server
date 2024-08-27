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
    ILogger         *logger = new Logger();
    IHandler        *handler = new Handler();
    IBuilderServer  *builder = new BuilderServer(handler, logger);
    Parser          parser(logger, handler, builder);
    std::string     fileName = "./config/ravy.conf";

    if (ac > 1) {
        fileName = av[1];
    }
    try {
        Http            http(logger);
        std::cout << logger->Log(&Logger::LogInformation, "Starting Parser.") << std::endl;
        parser.ConfigHttp(http, fileName);
        std::cout << logger->Log(&Logger::LogInformation, "Finished Parser.") << std::endl;
        http.StartPollList();
        http.StartWatchSockets();
    } catch (const std::exception &e) {
        std::cout << e.what() << std::endl;
    }
    delete builder;
    delete handler;
    delete logger;
    return (0);
}