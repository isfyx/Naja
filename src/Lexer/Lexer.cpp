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
            char c, delim = s_curline[s_colno];
            size_t len = 1;
            while (c) {
                if (c == '\\') {
                    len++;
                    c = s_curline[s_colno+len];
                    
                    if (!(s_is_echar(c) || s_is_oct(c))) {
                        std::cerr << "Lexer error on line "
                                  << s_lineno
                                  << ": Unrecognized escape sequence '\\"
                                  << c << "'." << std::endl;
                        exit(1);
                    } 
                    
                    if (s_is_echar(c)) {
                        if (c == 'x') {
                            len++;
                            c = s_curline[s_colno+len];
                            if (!s_is_hex(c)) {
                                std::cerr << "Lexer error on line "
                                          << s_lineno
                                          << ": Unrecognized escape sequence '\\x"
                                          << c << "'." << std::endl;
                                exit(1);
                            } else do {
                                len++;
                                c = s_curline[s_colno+len];
                            } while (s_is_hex(c));
                        } else if (c == 'u') {
                            for (size_t i = 0; i < 4; i++) {
                                len++;
                                c = s_curline[s_colno+len];
                                if (!s_is_hex(c)) {
                                    std::cerr << "Lexer error on line "
                                              << s_lineno
                                              << ": Escape sequence '\\u' must be followed "
                                              << "by exactly four hexadecimal digits."
                                              << std::endl;
                                    exit(1);
                                }
                            }
                        } else if (c == 'U') {
                            for (size_t i = 0; i < 8; i++) {
                                len++;
                                c = s_curline[s_colno+len];
                                if (!s_is_hex(c)) {
                                    std::cerr << "Lexer error on line "
                                              << s_lineno
                                              << ": Escape sequence '\\U' must be followed "
                                              << "by exactly eight hexadecimal digits."
                                              << std::endl;
                                    exit(1);
                                }
                            }
                        }
                    } else for (size_t i = 0; i < 3 && s_is_oct(c); i++) {
                        len++;
                        c = s_curline[s_colno+len];
                    }
                }
                if (c == delim) {
                    s_string = s_curline.substr(s_colno, len);
                    s_colno += len;
                    return Token::STRING;
                }
                if (!c) break;
                len++;
                c = s_curline[s_colno+len];
            }
            std::cerr << "Lexer error on line "
                      << s_lineno
                      << ": Unexpected end of line."
                      << std::endl;
            exit(1);                   
        }

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

    inline bool Lexer::s_is_echar(char& c)
    {
        return (  c == 'a'
               || c == 'b'
               || c == 'e'
               || c == 'f'
               || c == 'n'
               || c == 'r'
               || c == 't'
               || c == 'U'
               || c == 'u'
               || c == 'v'
               || c == 'x'
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