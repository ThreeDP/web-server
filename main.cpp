#include <iostream>
#include <sstream>
#include <string>

// Função que aceita apenas o primeiro argumento (caso base)
std::ostream printToStream(std::ostream& os) {
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

int main() {
    std::ostringstream oss;
    
    printToStream(oss, "The answer is", 42, "and the question is", "unknown");
    std::cout << oss.str() << std::endl;

    return 0;
}
