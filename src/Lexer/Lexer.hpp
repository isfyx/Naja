#pragma once

#include <cstdio>
#include <memory>

namespace Naja
{
    class Lexer
    {
    public:
        Lexer(const char* filename);
    private:
        std::unique_ptr<FILE> s_file;
    };
}