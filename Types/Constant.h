#pragma once

enum ConstantType
{
    CONST_INT,
    CONST_REAL,
    CONST_CHAR,
    CONST_STRING
};

class AbstactConstant
{
public:
    AbstactConstant(ConstantType type){this->type = type;}
    ConstantType getType() const {return this->type;}
    virtual ~AbstactConstant(){}
protected:
    ConstantType type;
};

template<typename T>
class Constant : public AbstactConstant
{
public:
    Constant(ConstantType type, T value);
    virtual ~Constant();
    T getValue() const;
private:
    T value;
};

// ------------------------- Implementation ------------------------

template<typename T>
Constant<T>::Constant(ConstantType type, T value) : AbstactConstant(type)
{
    this->value = value;
}

template<typename T>
Constant<T>::~Constant()
{}

template<typename T>
T Constant<T>::getValue() const
{
    return this->value;
}

