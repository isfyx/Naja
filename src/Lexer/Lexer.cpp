#include "Lexer.hpp"

#define N_STR_BUF_SIZE  65536

namespace Naja
{
    Lexer::Lexer(std::istream& istream)
        : s_istream (&istream)
        , s_string  ((char*) malloc(N_STR_BUF_SIZE))
        {}

    Lexer::~Lexer()
    {
        free(s_string);
    }

    Token Lexer::next()
    {}
}

#define N_STR_BUF_SIZE