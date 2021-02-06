#include "Token.h"
#include <sstream>
#include "PascalKeywords.h"
std::string AbstactToken::toString() const
{
    std::stringstream ss;
    switch(this->getType())
    {
        case TOKEN_IDENTIFIER: ss << "ident token: " << ((Token<std::string>*)this)->getData(); break;
        case TOKEN_CONST: ss << "const token: " << ((Token<AbstactConstant*>*)this)->getData()->toString(); break;
        case TOKEN_KEYWORD: ss << "keyword token: " << ((Token<PascalKeyword>*)this)->getData(); break;
        case TOKEN_UNKNOWN: ss << "unknown_token"; break;
    }
    return ss.str();
}
