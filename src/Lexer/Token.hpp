#pragma once

namespace Naja
{
    enum class TokenType
    { EOL       = '\n'
    , INDENT    = 256
    , DEDENT
    , INT
    , FLOAT
    , STRING
    , NAME
    };

    class Token
    {};
}