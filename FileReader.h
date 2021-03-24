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
    //���� ����� �����, �� ������ ��� \0
    //��������� ������ �� ��������� �������
    char readSymbol();
    //���������� ����� ������ �������
    char peekSymbol();
    size_t getRowIndex() const;
    size_t getColumnIndex() const;
    TextPosition getPosition();
    //����� �� ������� � ����� ����� ��������� �� ������
    void rollback(const TextPosition& position);
private:
    std::fstream fin;
    std::string row;
    size_t row_index;   //������� ����� ������
    size_t column_index;//����� �����
    std::map<size_t, size_t> row_limit;//���-�� �������� � ������
};
