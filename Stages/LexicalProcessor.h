#pragma once
#include <cstddef>
#include "../Types/Error.h"
#include "../Types/Token.h"
#include "../FileReader.h"
#include <map>

#include <string>



class LexicalProcessor
{
public:
    LexicalProcessor();
    ~LexicalProcessor();
    void bindReader(FileReader* reader);            //��������� �������� � ����������
    comp_error_t getNextToken(AbstactToken** token);//�������� ��������� �����
    void SaveTokenPosition(const std::string& reference);   //��������� ������� ������ + ���� �� ���
    void RollbackToToken(const std::string& reference);     //�������� ������ � ����������� ������
    void PopSavedTokenPosition(const std::string& reference);//������ ������ �� ������� ������
private:
    FileReader* reader;
    std::map<std::string, TextPosition> positions;
};

