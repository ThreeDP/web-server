#ifndef __LOGGER_HPP__
# define __LOGGER_HPP__

# include "ILogger.hpp"

void printToStream(std::ostream& os) {
    // Caso base, não faz nada
}

// Função sobrecarregada que aceita o primeiro argumento e depois chama recursivamente
template<typename T>
void printToStream(std::ostream& os, const T& first) {
    os << first;
}

// Função recursiva que aceita o primeiro argumento e a lista de argumentos restantes
template<typename T, typename... Args>
void printToStream(std::ostream& os, const T& first, const Args&... args) {
    os << first << " "; // Imprime o primeiro argumento
    printToStream(os, args...); // Chama recursivamente para os próximos argumentos
}

class Logger : public ILogger {
    public:
    Logger(void) {}
    ~Logger(void) {}

    void    LogInformation(std::string title, std::string message) {
        // std::cout << BLUB" [ " << Utils::getCurrentTimeInGMT() << " ]: " << title << " " << message << reset << std::endl;
    }

    void    LogWarning(std::stringstream title, std::stringstream message) {
        std::cout << YELB" [ " << title.str() << " ]: " reset << message.str() << std::endl;
    }

    void    LogError(std::stringstream title, std::stringstream message) {
        std::cerr << REDB" [ " << title.str() << " ]: " reset << message.str() << std::endl;
    }
};

#endif