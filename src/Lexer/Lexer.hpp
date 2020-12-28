#pragma once

#include <fstream>

#include "Token.hpp"

namespace Naja
{
    class Lexer
    {
    public:
        Lexer(const char* filename);
        ~Lexer();
        Token next();
    private:
        std::ifstream s_file;
    };
}