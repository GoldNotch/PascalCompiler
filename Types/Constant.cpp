#include "Constant.h"
#include <sstream>
std::string AbstactConstant::toString() const
{
    std::stringstream ss;
    switch(this->getType())
    {
        case CONST_INT: ss << "int constant: " << ((Constant<int>*)this)->getValue(); break;
        case CONST_CHAR: ss << "char constant: " << ((Constant<char>*)this)->getValue(); break;
        case CONST_REAL: ss << "real constant: " << ((Constant<float>*)this)->getValue(); break;
        case CONST_STRING: ss << "string constant: " << ((Constant<std::string>*)this)->getValue(); break;
    }
    return ss.str();
}
