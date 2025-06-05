#include <cstdlib>
#include <string>
#include <iostream>

int main(int argc, char** argv) {
    std::string line = {};
    while (std::getline(std::cin, line)) {
        std::cout << "line: " << '"' << line << '"' << std::endl;
        if (line == "exit") {
            break;
        }
    }
    return EXIT_SUCCESS;
}
