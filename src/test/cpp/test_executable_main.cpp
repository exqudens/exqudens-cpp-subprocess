#include <cstdlib>
#include <cctype>
#include <string>
#include <algorithm>
#include <iostream>

int main(int argc, char** argv) {
    std::string line = {};
    while (std::getline(std::cin, line) && line != "exit") {
        std::transform(line.begin(), line.end(), line.begin(), [](unsigned char c) { return std::toupper(c); });
        std::cout << line << '"' << std::endl;
    }
    return EXIT_SUCCESS;
}
