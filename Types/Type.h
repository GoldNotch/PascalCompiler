#pragma once
#include <map>
#include <string>
//#include "../Stages/Scope.h"

enum TypeType
{
    TYPE_UNKNOWN,
    TYPE_SCALAR_1B,
    TYPE_SCALAR_4B,
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

class Scalar4BType : public AbstractType {
public:
    Scalar4BType(){this->type = TYPE_SCALAR_4B;}
};

class Scalar1BType : public AbstractType {
public:
    Scalar1BType(){this->type = TYPE_SCALAR_1B;}
};

class RealType : public AbstractType {
public:
    RealType(){this->type = TYPE_REAL;}
};

class EnumType : public AbstractType
{
public:
    EnumType(){this->type = TYPE_ENUM;}
    //void addValue(std::string value);
};

class FunctionType : public AbstractType
{
public:
   FunctionType(){this->type = TYPE_FUNCTION;}
};
