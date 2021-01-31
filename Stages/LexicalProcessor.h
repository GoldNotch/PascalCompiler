#pragma once
#include <cstddef>
#include "../Compiler.h"
#include "../Types/Token.h"

class LexicalProcessor
{
public:
    LexicalProcessor();
    ~LexicalProcessor();
    void bindText(const char* text);            //��������� ����� � ����������
    comp_error_t getNextToken(AbstactToken** token);//�������� ��������� �����
private:
    const char* text;               //����������� ����� (������ ������)
    size_t last_token_position;     //������� ������ ���������� ������������ ������
    size_t current_row;             //������� ������
    size_t current_column;          //������� ������� � ������
};

