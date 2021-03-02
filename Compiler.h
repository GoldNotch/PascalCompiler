#pragma once
#include <cstddef>
#include <vector>
#include "Stages/LexicalProcessor.h"
#include "Types/Token.h"
#include "Types/PascalKeywords.h"

class Compiler
{
public:
    Compiler();
    ~Compiler();
    void bindReader(FileReader* reader);
    ErrorsArray Compile(void* const program);
private:
    bool readToken();
    AbstactToken* last_token; //������ ��� ����������� �����
    AbstactToken* next_token; //��������� (��� �� �����������) �����
    comp_error_t next_error;    //������ ���������� (��� �� ������������) ������
    LexicalProcessor lexer;

    bool isNextTokenCorrect();
    void SkipTokens(const std::vector<PascalKeyword>& stop_words, bool or_before_ident);
    bool isNextTokenOperator();
    bool isNextTokenCompareOperator();
    // ���� ������
    std::vector<comp_error_t> errors;
    void addError(ErrorType error_code);

    // ����������
    bool compile_Program         ();
    bool compile_Block           ();
    bool compile_VarSection      ();
    bool compile_VarDeclaration  ();
    bool compile_OperatorSection ();
    bool compile_CompoundOperator();
    bool compile_Operator        ();
    bool compile_IfOperator      ();
    bool compile_WhileOperator   ();
    bool compile_Assignment      ();
    bool compile_Expression      ();
    bool compile_SimpleExpression();
    bool compile_Term            ();   //���������
    bool compile_Multiplier      ();  //���������

    //�������������� �������
    bool compile_TypeSection     ();


};




