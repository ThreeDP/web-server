#include "./Parser/Parser.hpp"

int main(int ac, char **av) {
    if (ac != 2) {
        std::cout << "Pass the configuration file." << std::endl;
    }
    try {
        Parser Parser(av[1]);
    } catch (const std::exception &e) {
        std::cout << e.what() << std::endl;
    }
}

