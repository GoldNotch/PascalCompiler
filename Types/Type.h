#pragma once
#include <map>
#include <string>
#include "../Stages/Scope.h"

enum TypeType
{
    TYPE_UNKNOWN,
    TYPE_SCALAR,
    TYPE_REAL,
    TYPE_ENUM,
    TYPE_PROCEDURE,
    TYPE_FUNCTION,
    TYPE_ARRAY,
    TYPE_RECORD
};

class AbstractType
{
public:
    TypeType getType() const
    {return this->type;}
protected:
    TypeType type = TYPE_UNKNOWN;
};

class ScalarType : public AbstractType {
public:
    ScalarType(){this->type = TYPE_SCALAR;}
};
class RealType : public AbstractType {
public:
    RealType(){this->type = TYPE_REAL;}
};

