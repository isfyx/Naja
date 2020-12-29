#include <cerrno>
#include <cstring>
#include <iostream>
#include <fstream>
#include <memory>

#include "Lexer/Lexer.hpp"

using namespace Naja;

int main(int argc, char** argv)
{
    std::ifstream file;
    std::istream* in;
    char*         filename;
    if (argc > 1) {
        file.open(argv[1]);
        if (!file.is_open()) {
            std::cerr << "Failed to open '"
                      << argv[1]
                      << "': "
                      << std::strerror(errno)
                      << std::endl;
            exit errno;
        }
        filename = argv[1];
        in = &file;
    } else {
        in = &std::cin;
        filename = "stdin";
    }

    Lexer lexer(*in, filename);
    Token currentToken;
    while ((currentToken = lexer.next()) != Token::_EOF)
        switch (currentToken) {
            case Token::INDENT:
                std::cout << "INDENT ";
                break;
            case Token::DEDENT:
                std::cout << "DEDENT ";
                break;
            case Token::EOL:
                std::cout << "EOL" << std::endl;
                break;
            case Token::STRING:
                std::cout << "\""
                          << lexer.get_string_value()
                          << "\" ";
                break;
            default:;
        }
    std::cout << "EOF" << std::endl;
}