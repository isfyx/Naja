#pragma once

#include <cstdio>
#include <memory>

#include "Token.hpp"

namespace Naja
{
    class Lexer
    {
    public:
        Lexer(const char* filename);
        Token next();
    private:
        std::unique_ptr<FILE> s_file;
    };
}