#pragma once

#include <iostream>
#include <stack>

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

    typedef struct s_dedent_t s_dedent_t;

    class Lexer
    {
    public:
        Lexer(std::istream& istream, std::string filename);
        ~Lexer();
        
        inline long        get_int_value()    const  { return s_int;    }
        inline double      get_float_value()  const  { return s_float;  }
        inline std::string get_string_value() const  { return s_string; }
        
        Token next();
    private:
        void s_readline();
        bool s_is_echar(char& c);
        bool s_is_hex(char& c);
        bool s_is_num(char& c);
        bool s_is_oct(char& c);
        bool s_is_sign(char& c);
        bool s_is_whitespace(char& c);

        std::istream*       s_istream;
        size_t              s_lineno;
        size_t              s_colno;
        size_t              s_indent;
        std::stack<size_t>  s_dedents;
        bool                s_eol;
        std::string         s_curfilename;
        std::string         s_curline;
        long                s_int;
        double              s_float;
        std::string         s_string;
    };
}