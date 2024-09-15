/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/03 17:50:41 by dapaulin          #+#    #+#             */
/*   Updated: 2024/09/08 22:51:04 by user42           ###   ########.fr       */
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
        if (http != NULL)
            delete http;
        if (parser != NULL) {
            delete parser;
        }
    }
    exit(signum);
}

static void	setupSignal(void) {
	signal(SIGINT, signalHandler);
	signal(SIGTERM, signalHandler);
	signal(SIGQUIT, signalHandler);
	signal(SIGPIPE, SIG_IGN);
}

void setupNegation(void) {
    struct sigaction sa;
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);
}

int main(int ac, char *av[]) {
    setupSignal();
    ILogger         *logger = new Logger();
    IHandler        *handler = new Handler();
    IBuilderServer  *builder = new BuilderServer(handler, logger);
    parser = new Parser(logger, handler, builder);

    const char*     fileName = "./config/ravy.conf";
    if (ac > 1) {
        fileName = av[1];
    }
    try {
        
        http = new Http(logger);
        std::cout << logger->Log(&Logger::LogInformation, "Starting Parser.");
        setupNegation();
        parser->ConfigHttp(*http, fileName);
        std::cout << logger->Log(&Logger::LogInformation, "Finished Parser.");
        setupSignal();
        http->Process();
    } catch (const std::exception &e) {
        delete http;
        delete parser;
        std::cout << e.what();
    }
    return (0);
}