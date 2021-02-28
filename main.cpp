#include <cstdio>
#include <cstddef>
#include <cstdlib>
#include "Compiler.h"

//Получить размер файла
size_t GetFileSize(FILE* f)
{
    fpos_t cur_pos;
    fgetpos(f, &cur_pos);
    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    fseek(f, cur_pos, SEEK_SET);
    return size;
}

void printError(const comp_error_t* error)
{
    const char* explain = NULL;
    switch(error->code)
    {
        case LEX_ERROR_OUT_OF_RANGE: explain = "too long number for variable"; break;
        case SYN_ERROR_EXPECTED_ASSIGNMENT: explain = "expected \':=\'"; break;
        case SYN_ERROR_EXPECTED_BEGIN: explain = "expected \'begin\'"; break;
        case SYN_ERROR_EXPECTED_COLON: explain = "expected \':\'"; break;
        case SYN_ERROR_EXPECTED_DO: explain = "expected \'do\'"; break;
        case SYN_ERROR_EXPECTED_DOT: explain = "expected \'n\'"; break;
        case SYN_ERROR_EXPECTED_END: explain = "expected \'end\'"; break;
        case SYN_ERROR_EXPECTED_IDENTIFIER: explain = "expected identifier"; break;
        case SYN_ERROR_EXPECTED_PROGRAM: explain = "expected \'program\'"; break;
        case SYN_ERROR_EXPECTED_RIGHT_PARENTHESIS: explain = "expected \')\'"; break;
        case SYN_ERROR_EXPECTED_SEMICOLON: explain = "expected \';\'"; break;
        case SYN_ERROR_EXPECTED_THEN: explain = "expected \'then\'"; break;
        case SYN_ERROR_INVALID_OPERATOR: explain = "error in operator"; break;
        default: explain = "unknown error"; break;
    }
    printf("ERROR %i(%i, %i): %s\n", error->code, error->row, error->col, explain);
}

int main(int argc, char* argv[])
{
    FileReader reader("input.txt");
    void* program;
    Compiler compiler;
    compiler.bindReader(&reader);
    std::vector<comp_error_t> errors = compiler.Compile(program);
    for(size_t i = 0; i < errors.size(); i++)
    {
        printError(&errors[i]);
    }
    system("pause");
    return 0;
}
