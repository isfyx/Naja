#include <cerrno>
#include <cstring>
#include <iostream>
#include <fstream>

#include "Lexer/Lexer.hpp"

int main(int argc, char** argv)
{
    Naja::Lexer* lexer = nullptr;
    
    if (argc > 1) {
        std::ifstream in(argv[1]);
        if (in.is_open())
            lexer = new Naja::Lexer(in);
        else {
            std::cerr << "Failed to open '"
                      << argv[1]
                      << "': "
                      << std::strerror(errno)
                      << std::endl;
            exit(errno);
        }
    } else {
        lexer = new Naja::Lexer(std::cin);
    }

    while (lexer->next());
    
    delete lexer;
}