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
    AbstactToken(TokenType type, size_t row, size_t column)
    {
        this->Type = type;
        this->row = row;
        this->column = column;
    }
    TokenType getType() const {return this->Type;}
    size_t getRow() const {return row;}
    size_t getColumn() const {return column;}
    std::string toString() const;
    virtual ~AbstactToken(){}
protected:
    TokenType Type;
    //позиция токена в тексте
    size_t column;
    size_t row;
};
template <class T>
class Token : public AbstactToken
{
public:
    Token(TokenType type, size_t row, size_t column, const T& data);
    virtual ~Token();
    T getData() const;
protected:
    T data;
};

// ------------------------- Implementation ------------------------

template<class T>
Token<T>::Token(TokenType type, size_t row, size_t column, const T& data) : AbstactToken(type, row, column)
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
