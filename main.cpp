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



int main(int argc, char* argv[])
{
    FILE* in = fopen("input.txt", "r");
    if (in){
        size_t file_size = GetFileSize(in);
        char* text_program = (char*)malloc(file_size + 1);
        if (fread(text_program, sizeof(char), file_size, in) == 0)
        {
            printf("there's an error");
            return -1;
        }
        text_program[file_size] = '\0';

        void* program;
        std::vector<comp_error_t> errors = Compile(text_program, program);
        for(size_t i = 0; i < errors.size(); i++)
        {
            printf("ERROR: %i(%i, %i)\n", errors[i].code, errors[i].row, errors[i].col);
        }

        free(text_program);
        fclose(in);
    }
    else printf("file not found");
    system("pause");
    return 0;
}
