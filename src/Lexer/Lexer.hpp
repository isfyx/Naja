#pragma once

#include <iostream>
#include <memory>

#include "Token.hpp"

namespace Naja
{
    class Lexer
    {
    public:
        Lexer(std::istream& in);
        std::unique_ptr<Token> next();
    private:
        std::istream* s_in;
    };
}