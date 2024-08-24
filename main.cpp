#include <iostream>
#include <sstream>
#include <string>

void Logging(std::ostream& os) { }

template<typename T>
void Logging(std::ostream& os, const T& first) {
    os << first;
}

template<typename T, typename... Args>
void Logging(std::ostream& os, const T& first, const Args&... args) {
    os << first << " ";
    Logging(os, args...);
}

template<typename... Args>
std::string sformat(const Args&... args) {
    std::ostringstream oss;
    Logging(oss, args...);
    return oss.str();
}

int main() {
    std::cout << sformat("bea", 42, " ta dormindo? ", " Que foi?") << std::endl;
    return 0;
}
