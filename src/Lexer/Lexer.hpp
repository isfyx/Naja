#pragma once

#include <iostream>

#include "Token.hpp"

namespace Naja
{
    class Lexer
    {
    public:
        Lexer(std::istream& in);
        Token next();
    private:
        std::istream* s_in;
    };
}