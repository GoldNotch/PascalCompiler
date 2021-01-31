#pragma once
#include <cstddef>
#include <string>
#include "Constant.h"

enum TokenType
{
    TOKEN_UNKNOWN,
    TOKEN_IDENTIFIER,
    TOKEN_CONST,
    TOKEN_KEYWORD
};

class AbstactToken
{
public:
    AbstactToken(TokenType type){this->Type = type;}
    TokenType getType() const {return this->Type;}
    virtual ~AbstactToken(){}
protected:
    TokenType Type;
};
template <class T>
class Token : public AbstactToken
{
public:
    Token(TokenType type, const T& data);
    virtual ~Token();
    T getData() const;
protected:
    T data;
};

// ------------------------- Implementation ------------------------

template<class T>
Token<T>::Token(TokenType type, const T& data) : AbstactToken(type)
{
    this->data = data;
}

template<class T>
Token<T>::~Token()
{}

template<class T>
T Token<T>::getData() const
{
    return this->data;
}
