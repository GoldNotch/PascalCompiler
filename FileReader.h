#pragma once
#include <fstream>
#include <string>

class FileReader
{
public:
    FileReader(const char* path);
    ~FileReader();
    //если конец файла, то вернет нам \0
    //прочитать символ со смещением каретки
    char readSymbol();
    //посмотреть какой символ впереди
    char peekSymbol();
    size_t getRowIndex();
    size_t getColumnIndex();
private:
    std::fstream fin;
    std::string row;
    size_t row_index;   //текущий номер строки
    size_t column_index;//номер буквы
};
