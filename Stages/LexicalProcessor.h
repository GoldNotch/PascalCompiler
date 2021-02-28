#pragma once
#include <cstddef>
#include "../Types/Error.h"
#include "../Types/Token.h"
#include "../FileReader.h"

#include <string>

class LexicalProcessor
{
public:
    LexicalProcessor();
    ~LexicalProcessor();
    void bindReader(FileReader* reader);            //��������� �������� � ����������
    comp_error_t getNextToken(AbstactToken** token);//�������� ��������� �����
private:
    FileReader* reader;
};

