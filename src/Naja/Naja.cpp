#include <cerrno>
#include <cstring>
#include <iostream>
#include <fstream>
#include <memory>

#include "Lexer/Lexer.hpp"
#include "Lexer/Token.hpp"

using namespace Naja;

int main(int argc, char** argv)
{
    std::unique_ptr<Lexer> lexer;
    
    if (argc > 1) {
        std::ifstream in(argv[1]);
        if (in.is_open())
            lexer = std::move(std::make_unique<Lexer>(in));
        else {
            std::cerr << "Failed to open '"
                      << argv[1]
                      << "': "
                      << std::strerror(errno)
                      << std::endl;
            exit(errno);
        }
    } else
        lexer = std::move(std::make_unique<Lexer>(std::cin));

    Token currentToken;
    while ((currentToken = lexer->next()) != Token::_EOF)
        std::cout << (int)currentToken << std::endl;
}