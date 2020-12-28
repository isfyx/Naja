#include "Lexer.hpp"

namespace Naja
{
    Lexer::Lexer(const char* filename)
    {
        std::unique_ptr<FILE> file(fopen(filename, "r"));
        s_file = std::move(file);
    }
}