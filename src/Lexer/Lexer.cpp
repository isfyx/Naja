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
        if (s_curline[s_colno] == '\'' || s_curline[s_colno] == '\"') {
            char delim = s_curline[s_colno];
            s_string.clear();
            s_colno++;
            while (s_curline[s_colno]) {
                if (s_curline[s_colno] == delim) {
                    s_colno++;
                    return Token::STRING;
                }
                if (s_curline[s_colno] == '\\') {
                    s_colno++;
                    char   c   = s_curline[s_colno];
                    size_t len = 1;
                    if (s_is_echar(c)) {
                        switch (c) {
                            case 'a'  : c = '\a'; break;
                            case 'b'  : c = '\b'; break;
                            case 'e'  : c = '\e'; break;
                            case 'f'  : c = '\f'; break;
                            case 'n'  : c = '\n'; break;
                            case 'r'  : c = '\r'; break;
                            case 't'  : c = '\t'; break;
                            case 'v'  : c = '\v'; break;
                            case '\\' : c = '\\'; break;
                            case '\'' : c = '\''; break;
                            case '\"' : c = '\"'; break;
                            case '\?' : c = '\?'; break;
                        }
                    } else if (s_is_oct(c)) {
                        c = stoi(s_curline.substr(s_colno, 3), &len, 8);
                    // } else if (c == 'u') {
                    //     s_colno++;
                    //     int ucn = stoi(s_curline.substr(s_colno, 4), &len, 16);
                    //     if (len != 4) {
                    //         std::cerr << "Lexer error on line "
                    //                   << s_lineno
                    //                   << ": Escape sequence '\\u' must be "
                    //                   << "followed by exactly four hexadecimal "
                    //                   << "digits."
                    //                   << std::endl;
                    //         exit(1); 
                    //     }
                    //     s_string += (char)(ucn/0xff);
                    //     c = (char)(ucn%0xff);
                    // } else if (c == 'U') {
                    //     s_colno++;
                    //     long ucn = stol(s_curline.substr(s_colno, 8), &len, 16);
                    //     if (len != 8) {
                    //         std::cerr << "Lexer error on line "
                    //                   << s_lineno
                    //                   << ": Escape sequence '\\U' must be "
                    //                   << "followed by exactly eight hexadecimal "
                    //                   << "digits."
                    //                   << std::endl;
                    //         exit(1); 
                    //     }
                    //     s_string += (char)(ucn/0xffffff);
                    //     s_string += (char)((ucn/0xffff)%0xff);
                    //     s_string += (char)((ucn/0xff)%0xffff);
                    //     c = (char)(ucn%0xff);
                    } else if (c == 'x') {
                        s_colno++;
                        c = stoi(s_curline.substr(s_colno, 2), &len, 16);
                    } else {
                        std::cerr << "Lexer error on line "
                                  << s_lineno
                                  << ": Invalid escape sequence '\\"
                                  << c << "'." << std::endl;
                        exit(1);
                    }
                    s_string += c;
                    s_colno  += len;
                } else {
                    s_string += s_curline[s_colno++];
                }
            }
            std::cerr << "Lexer error on line "
                      << s_lineno
                      << ": Invalid string literal."
                      << std::endl;
            exit(1);
        }

        // Handle Symbols
        // TODO
        
        // Handle keywords
        {
            size_t len;

            if ((len = s_keyword("def"))) {
                s_colno += len;
                return Token::DEFINE;
            }
            if ((len = s_keyword("if"))) {
                s_colno += len;
                return Token::IF;
            }
            if ((len = s_keyword("else"))) {
                s_colno += len;
                return Token::ELSE;
            }
        }

        // Handle names
        // TODO

        s_string = s_curline.substr(s_colno);
        s_colno += s_string.size();

        return Token::STRING;
    }

    size_t Lexer::s_keyword(const char* keyword)
    {
        size_t len  = strlen(keyword);
        char   next = s_curline[s_colno+len];
        if (!next || !(s_is_whitespace(next) || next == '.'))
            return 0;
        return s_curline.substr(s_colno).rfind(keyword, 0) == 0 ? len : 0;
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

    inline bool Lexer::s_is_echar(char& c)
    {
        return (  c == 'a'
               || c == 'b'
               || c == 'e'
               || c == 'f'
               || c == 'n'
               || c == 'r'
               || c == 't'
               || c == 'v'
               || c == '\\'
               || c == '\''
               || c == '\"'
               || c == '\?'
               );
    }

    inline bool Lexer::s_is_hex(char& c)
    {
        return (  s_is_num(c)
               || ('a' <= c && c <= 'f')
               || ('A' <= c && c <= 'F')
               );
    }

    inline bool Lexer::s_is_num(char& c)
    {
        return ('0' <= c && c <= '9');
    }

    inline bool Lexer::s_is_oct(char& c)
    {
        return ('0' <= c && c <= '7');
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