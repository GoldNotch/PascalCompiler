#pragma once
#include <vector>
enum ErrorType
{
    NO_ERRORS,
    LEX_ERROR_OUT_OF_RANGE,  //выход за диапазон числа
    SYN_ERROR_EXPECTED_PROGRAM,
    SYN_ERROR_EXPECTED_SEMICOLON,
    SYN_ERROR_EXPECTED_IDENTIFIER,
    SYN_ERROR_EXPECTED_COLON,
    SYN_ERROR_EXPECTED_BEGIN,
    SYN_ERROR_EXPECTED_END,
    SYN_ERROR_EXPECTED_DOT,
    SYN_ERROR_EXPECTED_ASSIGNMENT,
    SYN_ERROR_EXPECTED_RIGHT_PARENTHESIS,
    SYN_ERROR_EXPECTED_THEN,
    SYN_ERROR_EXPECTED_DO,
    SYN_ERROR_INVALID_OPERATOR,
};

struct comp_error_t
{
    int code;
    size_t row;
    size_t col;
};

typedef std::vector<comp_error_t> ErrorsArray;

static void mergeErrorsArrays(ErrorsArray* arr1, const ErrorsArray& arr2)
{
    arr1->insert(arr1->end(), arr2.begin(), arr2.end());
}
