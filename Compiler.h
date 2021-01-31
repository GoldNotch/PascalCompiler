#pragma once
#include <cstddef>
#include <vector>

enum ErrorType
{
    NO_ERRORS,
    LEX_ERROR_END_OF_FILE,  //����� �����
    LEX_ERROR_OUT_OF_INT_RANGE  //����� �� �������� �����
};

struct comp_error_t
{
    int code;
    size_t row;
    size_t col;
};

std::vector<comp_error_t> Compile(const char* text, void* const program);


