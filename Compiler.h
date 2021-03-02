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
    AbstactToken* last_token; //только что прочитанный токен
    AbstactToken* next_token; //следующий (еще не прочитанный) токен
    comp_error_t next_error;    //ошибка следующего (еще не прочитанного) токена
    LexicalProcessor lexer;

    bool isNextTokenCorrect();
    void SkipTokens(const std::vector<PascalKeyword>& stop_words, bool or_before_ident);
    bool isNextTokenOperator();
    bool isNextTokenCompareOperator();
    // сбор ошибок
    std::vector<comp_error_t> errors;
    void addError(ErrorType error_code);

    // Грамматики
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
    bool compile_Term            ();   //слагаемое
    bool compile_Multiplier      ();  //множитель

    //Индивидуальное задание
    bool compile_TypeSection     ();


};




