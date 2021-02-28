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
    AbstactToken* last_token; //только что прочитанный токен
    AbstactToken* next_token; //следующий (еще не прочитанный) токен
    comp_error_t next_error;    //ошибка следующего (еще не прочитанного) токена
    LexicalProcessor lexer;

    bool isNextTokenCorrect();
    comp_error_t createError(ErrorType error_code);
    void addError(ErrorType error_code, ErrorsArray* errors);
    void SkipTokens(const std::vector<PascalKeyword>& stop_words, bool or_before_ident);
    bool isNextTokenOperator();
    bool isNextTokenCompareOperator();

    // Грамматики
    ErrorsArray compile_Program         ();
    ErrorsArray compile_Block           ();
    ErrorsArray compile_VarSection      ();
    ErrorsArray compile_OperatorSection ();
    ErrorsArray compile_CompoundOperator();
    ErrorsArray compile_Operator        ();
    ErrorsArray compile_IfOperator      ();
    ErrorsArray compile_WhileOperator   ();
    //Индивидуальное задание
    ErrorsArray compile_TypeSection     ();

    comp_error_t compile_VarDeclaration ();
    comp_error_t compile_Assignment     ();
    comp_error_t compile_Expression     ();
    comp_error_t compile_SimpleExpression();
    comp_error_t compile_Term           ();   //слагаемое
    comp_error_t compile_Multiplier     ();  //множитель


};




