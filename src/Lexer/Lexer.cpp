#include "Lexer.hpp"

#include <cerrno>
#include <cstring>

#define N_STR_BUF_SIZE  65536

namespace Naja
{
    struct s_dedent_t {
        size_t      indent;
        s_dedent_t* next;
    };

    Lexer::Lexer(std::istream& istream, char* filename)
        : s_istream         (&istream)
        , s_lineno          (0)
        , s_colno           (0)
        , s_indent          (0)
        , s_dedent          (0)
        , s_eol             (true)
        , s_curfilename     (filename)
        , s_curline         ((char*) calloc(N_STR_BUF_SIZE, sizeof(char)))
        , s_string          ((char*) calloc(N_STR_BUF_SIZE, sizeof(char)))
        {}

    Lexer::~Lexer()
    {
        free(s_curline);
        free(s_string);
    }

    Token Lexer::next()
    {
        // Get the next line if needed
        if (s_eol && !s_istream->eof())
            s_readline();
        
        // Deal with the end of file
        if (s_eol && s_istream->eof()) {
            while (s_indent > 0) {
                s_dedent_t* old = s_dedent;
                s_indent = s_dedent->indent;
                s_dedent = s_dedent->next;
                delete old;
                return Token::DEDENT;
            }
            return Token::_EOF;
        }

        // Deal with indentation
        if (!s_colno) {
            while (s_is_whitespace(s_curline[s_colno]))
                s_colno++;
        
            if (s_colno > s_indent) {
                s_dedent = new s_dedent_t {s_indent, s_dedent};
                s_indent = s_colno;
                s_colno  = 0;
                return Token::INDENT;
            }

            if (s_colno < s_indent) {
                s_dedent_t* old = s_dedent;
                s_indent = s_dedent->indent;
                s_dedent = s_dedent->next;
                s_colno  = 0;
                delete old;
                return Token::DEDENT;
            }
        }

        // Ignore whitespace
        while (s_is_whitespace(s_curline[s_colno]))
            s_colno++;

        // Deal with comments and end of line
        if (!s_curline[s_colno] || s_curline[s_colno] == '#') {
            s_eol = true;
            return Token::EOL;
        }
        
        strcpy(s_string, &s_curline[s_colno]);
        s_colno += strlen(s_string);

        return Token::STRING;
    }

    void Lexer::s_readline()
    {
        if (s_istream->eof())
            return;

        s_istream->getline(s_curline, N_STR_BUF_SIZE);
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

    inline bool Lexer::s_is_whitespace(char& c)
    {
        return (c == ' ' || c == '\t');
    }
}