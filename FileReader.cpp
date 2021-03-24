#include "FileReader.h"
#include <cstdio>

FileReader::FileReader(const char* path) : fin(path)
{
    row_index = 0;
    column_index = 0;
    std::getline(fin, row);
    //пропуск пустых строк
    while(row.empty() && fin.peek() != EOF)
    {
        row_limit[row_index] = 0;
        std::getline(fin, row);
        row_index++;
    }
}

FileReader::~FileReader()
{
    fin.close();
    row_limit.clear();
    row = "";
}

char FileReader::readSymbol()
{
    if (this->column_index >= this->row.length())
    {
        if (fin.peek() == EOF)
            return '\0';
        else{
            row_limit[row_index] = column_index + 1;
            column_index = 0;
            row_index++;
            std::getline(fin, row);
            while (row.empty() && fin.peek() != EOF)
            {
                row_limit[row_index] = 0;
                std::getline(fin, row);
                row_index++;
            }
        }
    }
    return this->row[this->column_index++];
}

char FileReader::peekSymbol()
{
    if (column_index < row.length())
        return row[column_index];
    else return fin.peek() == EOF ? '\0' : '\n';
}

size_t FileReader::getRowIndex() const
{
     return row_index + 1;
}

size_t FileReader::getColumnIndex() const
{
    return column_index + 1;
}

TextPosition FileReader::getPosition()
{
    return {getRowIndex() - 1, fin.tellg() - this->row.length() + 1, getColumnIndex() - 1};
}

void FileReader::rollback(const TextPosition& position)
{
    fin.seekg(position.row_file_offset, fin.beg);
    std::getline(fin, row);
    this->row_index = position.row_index;
    this->column_index = position.col_index;
}
