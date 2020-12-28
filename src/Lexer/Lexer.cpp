#include "Lexer.hpp"

namespace Naja
{
    Lexer::Lexer(const char* filename)
    {
        s_file.open(filename);
    }

    Lexer::~Lexer()
    {
        s_file.close();
    }

    Token Lexer::next()
    {

    }
}