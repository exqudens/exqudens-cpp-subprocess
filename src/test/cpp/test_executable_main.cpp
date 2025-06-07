#include <cstdlib>
#include <cctype>
#include <string>
#include <algorithm>
#include <iostream>

int main(int argc, char** argv) {
    std::string line = {};
    bool toOut = true;
    while (std::getline(std::cin, line) && line != "exit") {
        if (line == "stdout") {
            toOut = true;
            continue;
        } else if (line == "stderr") {
            toOut = false;
            continue;
        }
        std::transform(line.begin(), line.end(), line.begin(), [](unsigned char c) { return std::toupper(c); });
        if (toOut) {
            std::cout << line << std::endl;
        } else {
            std::cerr << line << std::endl;
        }
    }
    return EXIT_SUCCESS;
}
