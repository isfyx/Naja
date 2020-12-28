#pragma once

namespace Naja
{
    enum class TokenType
    { EOF       = 0
    , EOL       = '\n'
    , NULL      = 256
    , INT
    , FLOAT
    , STRING
    , NAME
    };
}