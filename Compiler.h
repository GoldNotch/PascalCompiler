#pragma once
#include <cstddef>
#include <vector>
#include "Stages/LexicalProcessor.h"
#include "Types/Token.h"
#include "Types/PascalKeywords.h"
#include "Stages/Scope.h"
#include "Stages/CodeGenerator.h"
#include <stack>

class Compiler
{
public:
    Compiler();
    ~Compiler();
    void bindReader(FileReader* reader);
    ErrorsArray Compile(const char* path);
private:
    bool readToken();
    AbstactToken* last_token;           //������ ��� ����������� �����
    AbstactToken* next_token;           //��������� (��� �� �����������) �����
    comp_error_t next_error;            //������ ���������� (��� �� ������������) ������
    LexicalProcessor lexer;             //����������� ����������
    Scope *global_scope;                //���������� ������� ���������
    AbstractType* last_compiled_type;   //��������� ���������������� ���
    Data* last_compiled_variable;       //��������� ���������������� ����������
    CodeGenerator* generator;
    std::stack<PascalKeyword> opstack;


    //������� �������
    bool isNextTokenCorrect();  //��������� ����� - ����������/����������
    bool isNextTokenOperator(); //��������� ����� - ������ ���������
    bool isNextTokenExpression(); //��������� ����� - ������ ���������
    bool isNextTokenType();     //��������� ����� - ������ ���� ������
    bool isNextTokenCompareOperator();  //��������� ����� - �������� ���������
    bool isNextTokenVariable();         //��������� ����� - ����������
    bool isAssignableTypes(AbstractType* t1, AbstractType* t2); //���� ����������

    //���� � ��������� ������
    std::vector<comp_error_t> errors;
    void addError(ErrorType error_code);
    void SkipTokens(const std::vector<PascalKeyword>& stop_words, bool before_ident, bool before_const);

    // ����������
    bool compile_Program            ();
    bool compile_Block              ();
    bool compile_VarSection         ();
    bool compile_VarDeclaration     ();
    bool compile_Type               ();
    bool compile_OperatorSection    ();
    bool compile_CompoundOperator   ();
    bool compile_Operator           ();
    bool compile_IfOperator         ();
    bool compile_WhileOperator      ();
    bool compile_Variable           ();
    bool compile_FunctionCall       ();
    bool compile_Assignment         ();
    bool compile_Expression         ();
    bool compile_SimpleExpression   ();
    bool compile_Term               ();  //���������
    bool compile_Multiplier         ();  //���������

    //�������������� �������
    bool compile_TypeSection        ();
    bool compile_TypeDeclatation    ();
    bool compile_Record             ();
    bool compile_WithOperator       ();
    bool compile_ForOperator        ();
    bool compile_RepeatOperator     ();


};




