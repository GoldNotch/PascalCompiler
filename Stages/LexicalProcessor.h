#pragma once
#include <cstddef>
#include "../Compiler.h"
#include "../Types/Token.h"

class LexicalProcessor
{
public:
    LexicalProcessor();
    ~LexicalProcessor();
    void bindText(const char* text);            //привязать текст к процессору
    comp_error_t getNextToken(AbstactToken** token);//получить следующий токен
private:
    const char* text;               //привязанный текст (слабая ссылка)
    size_t last_token_position;     //позиция начала последнего прочитанного токена
    size_t current_row;             //текущая строка
    size_t current_column;          //текущая позиция в строке
};

