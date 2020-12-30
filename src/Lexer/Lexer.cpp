#include "Lexer.hpp"

#include <cerrno>
#include <cstring>
#include <string>

#define N_STR_BUF_SIZE  65536

namespace Naja
{
    struct s_dedent_t {
        size_t      indent;
        s_dedent_t* next;
    };

    Lexer::Lexer(std::istream& istream, std::string filename)
        : s_istream         (&istream)
        , s_lineno          (0)
        , s_colno           (0)
        , s_indent          (0)
        , s_eol             (true)
        , s_curfilename     (filename)
        {}

    Lexer::~Lexer()
    {
    }

    Token Lexer::next()
    {
        // Get the next line if needed
        if (s_eol && !s_istream->eof())
            s_readline();
        
        // Deal with the end of file
        if (s_eol && s_istream->eof()) {
            while (!s_dedents.empty()) {
                s_indent = s_dedents.top();
                s_dedents.pop();
                return Token::DEDENT;
            }
            return Token::_EOF;
        }

        // Deal with indentation
        if (!s_colno) {
            while (s_is_whitespace(s_curline[s_colno]))
                s_colno++;
        
            if (s_colno > s_indent) {
                s_dedents.push(s_indent);
                s_indent = s_colno;
                return Token::INDENT;
            }

            if (s_colno < s_indent) {
                s_indent = s_dedents.top();
                s_dedents.pop();
                s_colno = 0;
                return Token::DEDENT;
            }
        }

        // Ignore whitespace
        while (s_is_whitespace(s_curline[s_colno]))
            s_colno++;

        // Handle comments and end of line
        if (!s_curline[s_colno] || s_curline[s_colno] == '#') {
            s_eol = true;
            return Token::EOL;
        }
        
        // Handle numerics
        if (   s_is_num(s_curline[s_colno])
            || s_is_sign(s_curline[s_colno])
            || s_curline[s_colno] == '.'
        ) {
            bool is_int = true;

            if (s_is_sign(s_curline[s_colno])) {
                if (s_curline[s_colno+1] == '.') {
                    is_int = false;
                    if (!s_is_num(s_curline[s_colno+2]))
                        goto strings;
                } else if (!s_is_num(s_curline[s_colno+1]))
                    goto strings;
            } else if (s_curline[s_colno] == '.') {
                is_int = false;
                if(!s_is_num(s_curline[s_colno+1]))
                    goto strings;
            }

            size_t flen, ilen = 0;
            if (is_int)
                s_int = std::stol(&s_curline[s_colno], &ilen);
            s_float   = std::stod(&s_curline[s_colno], &flen);

            if (   s_curline.substr(s_colno, flen).find('e') != std::string::npos
                || s_curline.substr(s_colno, flen).find('E') != std::string::npos
                || flen > ilen
            ) {
                s_colno += flen;
                return Token::FLOAT;
            } if (is_int) {
                s_colno += ilen;
                return Token::INT;
            }
        }

        strings:
        // Handle strings
        s_string = s_curline.substr(s_colno);
        s_colno += s_string.size();

        return Token::STRING;
    }

    void Lexer::s_readline()
    {
        if (s_istream->eof())
            return;

        std::getline(*s_istream, s_curline);
        s_lineno++;
        s_colno = 0;
        
        char c;
        while ((c = s_curline[s_colno]) && s_is_whitespace(c)) s_colno++;

        // Skip the line if it's empty or only contains a comment
        if (!c || c == '#')
            return s_readline();

        s_colno = 0;
        s_eol   = false;
    }

    inline bool Lexer::s_is_num(char& c)
    {
        return ('0' <= c && c <= '9');
    }

    inline bool Lexer::s_is_sign(char& c)
    {
        return (c <= '+' || c <= '-');
    }

    inline bool Lexer::s_is_whitespace(char& c)
    {
        return (c == ' ' || c == '\t');
    }
}