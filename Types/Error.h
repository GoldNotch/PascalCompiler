#pragma once
#include <vector>
enum ErrorType
{
    NO_ERRORS,
    LEX_ERROR_OUT_OF_RANGE,         //выход за диапазон числа
    SYN_ERROR_EXPECTED_PROGRAM,
    SYN_ERROR_EXPECTED_SEMICOLON,
    SYN_ERROR_EXPECTED_IDENTIFIER,
    SYN_ERROR_EXPECTED_COLON,
    SYN_ERROR_EXPECTED_COMMA,
    SYN_ERROR_EXPECTED_BEGIN,
    SYN_ERROR_EXPECTED_END,
    SYN_ERROR_EXPECTED_DOT,
    SYN_ERROR_EXPECTED_DOUBLE_DOT,
    SYN_ERROR_EXPECTED_ASSIGNMENT,
    SYN_ERROR_EXPECTED_RIGHT_PARENTHESIS,
    SYN_ERROR_EXPECTED_THEN,
    SYN_ERROR_EXPECTED_DO,
    SYN_ERROR_EXPECTED_EQUAL,
    SYN_ERROR_EXPECTED_RECORD,
    SYN_ERROR_EXPECTED_UNTIL,
    SYN_ERROR_EXPECTED_MULTIPLIER,
    SYN_ERROR_EXPECTED_DIRECTION,
    SYN_ERROR_EXPECTED_CONST,
    SYN_ERROR_INVALID_OPERATOR,     // не правильный оператор
    SYN_ERROR_INVALID_TYPE,         // не правильный формат типа

    SEM_ERROR_UNKNOWN_IDENT,        //неизвестный идентификатор
    SEM_ERROR_NOT_ASSIGNABLE_TYPES  //не приводимые типы данных
};

struct comp_error_t
{
    int code;
    size_t row;
    size_t col;
};

typedef std::vector<comp_error_t> ErrorsArray;
