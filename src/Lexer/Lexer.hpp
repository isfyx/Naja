#pragma once

#include <iostream>
#include <memory>

#include "Token.hpp"

namespace Naja
{
    enum class Token
    { _EOF      = -1
    , _NULL     = 0
    , EOL       = '\n'
    , INDENT    = 256
    , DEDENT    , INT
    , FLOAT     , STRING
    , NAME
    };

    class Lexer
    {
    public:
        Lexer(std::istream& istream);
        ~Lexer();
        
        inline long   get_int_value()    const  { return s_int;    }
        inline double get_float_value()  const  { return s_float;  }
        inline char*  get_string_value() const  { return s_string; }
        
        Token next();
    private:
        std::istream* s_istream;
        long          s_int;
        double        s_float;
        char*         s_string;
    };
}