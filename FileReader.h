#pragma once
#include <fstream>
#include <string>

class FileReader
{
public:
    FileReader(const char* path);
    ~FileReader();
    //���� ����� �����, �� ������ ��� \0
    //��������� ������ �� ��������� �������
    char readSymbol();
    //���������� ����� ������ �������
    char peekSymbol();
    size_t getRowIndex();
    size_t getColumnIndex();
private:
    std::fstream fin;
    std::string row;
    size_t row_index;   //������� ����� ������
    size_t column_index;//����� �����
};
