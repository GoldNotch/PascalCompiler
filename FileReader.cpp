#include "FileReader.h"
#include <cstdio>

FileReader::FileReader(const char* path) : fin(path)
{
    this->row_index = 0;
    this->column_index = 0;
    std::getline(fin, row);
    //пропуск пустых строк
    while(row.empty() && fin.peek() != EOF)
    {
        std::getline(fin, row);
        this->row_index++;
    }
}

FileReader::~FileReader()
{
    this->fin.close();
    this->row = "";
}

char FileReader::readSymbol()
{
    if (this->column_index >= this->row.length())
    {
        if (fin.peek() == EOF)
            return '\0';
        else{
            this->column_index = 0;
            this->row_index++;
            std::getline(fin, row);
            while (this->row.empty() && fin.peek() != EOF)
            {
                std::getline(fin, row);
                this->row_index++;
            }
        }
    }
    return this->row[this->column_index++];
}

char FileReader::peekSymbol()
{
    if (this->column_index < this->row.length())
        return this->row[this->column_index];
    else return fin.peek() == EOF ? '\0' : '\n';
}

size_t FileReader::getRowIndex()
{
     return this->row_index + 1;
}

size_t FileReader::getColumnIndex()
{
    return this->column_index + 1;
}
