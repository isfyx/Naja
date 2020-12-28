#include "Lexer.hpp"

namespace Naja
{
    Lexer::Lexer(std::istream& in)
        : s_in(&in) {}

    std::unique_ptr<Token> Lexer::next()
    {}
}