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
    comp_error_t readToken();
    AbstactToken* last_token; //������ ��� ����������� �����
    AbstactToken* next_token; //��������� (��� �� �����������) �����
    comp_error_t next_error;    //������ ���������� (��� �� ������������) ������
    LexicalProcessor lexer;

    bool isNextTokenCorrect();
    comp_error_t createError(ErrorType error_code);
    void addError(ErrorType error_code, ErrorsArray* errors);
    void SkipTokens(const std::vector<PascalKeyword>& stop_words, bool or_before_ident);
    bool isNextTokenOperator();
    bool isNextTokenCompareOperator();

    // ����������
    ErrorsArray compile_Program         ();
    ErrorsArray compile_Block           ();
    ErrorsArray compile_VarSection      ();
    ErrorsArray compile_OperatorSection ();
    ErrorsArray compile_CompoundOperator();
    ErrorsArray compile_Operator        ();
    ErrorsArray compile_IfOperator      ();
    ErrorsArray compile_WhileOperator   ();
    //�������������� �������
    ErrorsArray compile_TypeSection     ();

    comp_error_t compile_VarDeclaration ();
    comp_error_t compile_Assignment     ();
    comp_error_t compile_Expression     ();
    comp_error_t compile_SimpleExpression();
    comp_error_t compile_Term           ();   //���������
    comp_error_t compile_Multiplier     ();  //���������


};




