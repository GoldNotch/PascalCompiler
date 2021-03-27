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
    AbstactToken* last_token;           //только что прочитанный токен
    AbstactToken* next_token;           //следующий (еще не прочитанный) токен
    comp_error_t next_error;            //ошибка следующего (еще не прочитанного) токена
    LexicalProcessor lexer;             //лексический анализатор
    Scope *global_scope;                //глобальная область видимости
    AbstractType* last_compiled_type;   //последний скомпилированный тип
    Data* last_compiled_variable;       //последняя скомпилированная переменная
    CodeGenerator* generator;
    std::stack<PascalKeyword> opstack;


    //условия токенов
    bool isNextTokenCorrect();  //следующий токен - корректный/правильный
    bool isNextTokenOperator(); //следующий токен - начало оператора
    bool isNextTokenExpression(); //следующий токен - начало выражения
    bool isNextTokenType();     //следующий токен - начало типа данных
    bool isNextTokenCompareOperator();  //следующий токен - оператор сравнения
    bool isNextTokenVariable();         //следующий токен - переменная
    bool isAssignableTypes(AbstractType* t1, AbstractType* t2); //типы соотносимы

    //сбор и обработка ошибок
    std::vector<comp_error_t> errors;
    void addError(ErrorType error_code);
    void SkipTokens(const std::vector<PascalKeyword>& stop_words, bool before_ident, bool before_const);

    // Грамматики
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
    bool compile_Term               ();  //слагаемое
    bool compile_Multiplier         ();  //множитель

    //Индивидуальное задание
    bool compile_TypeSection        ();
    bool compile_TypeDeclatation    ();
    bool compile_Record             ();
    bool compile_WithOperator       ();
    bool compile_ForOperator        ();
    bool compile_RepeatOperator     ();


};




