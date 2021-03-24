#pragma once
#include <fstream>
#include <string>
#include <map>

struct TextPosition
{
    size_t row_index;
    size_t row_file_offset;
    size_t col_index;
};

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
    size_t getRowIndex() const;
    size_t getColumnIndex() const;
    TextPosition getPosition();
    //откат на позицию в файле чтобы прочитать ее заново
    void rollback(const TextPosition& position);
private:
    std::fstream fin;
    std::string row;
    size_t row_index;   //текущий номер строки
    size_t column_index;//номер буквы
    std::map<size_t, size_t> row_limit;//кол-во символов в строке
};
