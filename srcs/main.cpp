/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/03 17:50:41 by dapaulin          #+#    #+#             */
/*   Updated: 2024/09/07 12:31:05 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Http.hpp"
# include "Parser.hpp"
# include "BuilderServer.hpp"

#include <iostream>
#include <csignal>
#include <cstdlib>

Http *http = NULL;
Parser *parser = NULL;

void signalHandler(int signum) {
    if(signum == SIGINT){
        // close(Http::GetEPollFD());
        if (http != NULL)
            delete http;
        if (parser != NULL)
            delete parser;
    }
    exit(signum);
}

int main(int ac, char *av[]) {
    signal(SIGINT, signalHandler);
    ILogger         *logger = new Logger();
    IHandler        *handler = new Handler();
    IBuilderServer  *builder = new BuilderServer(handler, logger);
    parser = new Parser(logger, handler, builder);

    std::string     fileName = "./config/ravy.conf";
    
    if (ac > 1) {
        fileName = av[1];
    }
    try {
        
        http = new Http(logger);
        std::cout << logger->Log(&Logger::LogInformation, "Starting Parser.");
        parser->ConfigHttp(*http, fileName);
        std::cout << logger->Log(&Logger::LogInformation, "Finished Parser.");
        http->Process();
    } catch (const std::exception &e) {
        std::cout << e.what();
    }
    return (0);
}