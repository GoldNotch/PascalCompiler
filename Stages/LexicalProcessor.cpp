#include <cstdlib>
#include <cctype>
#include <algorithm>
#include "LexicalProcessor.h"
#include "../Types/PascalKeywords.h"

enum LexemeType
{
    LEXEME_UNKNOWN,
    LEXEME_IDENTIFIER,     //[A-Za-z_][\w_]*
    LEXEME_INT_CONST,   //\d+([.,]\d*)? 					-- неправильно, потому что еще бывают u, ull, ll, etc, а еще константа может начинаться с + и -
    LEXEME_REAL_CONST,
    LEXEME_STRING_CONST   //'any ascii symbol*'
  //[:=<>+\-*\/\(\)\{\}\[\]'^,;]+
};
comp_error_t ParseToken(LexemeType type, const std::string& lexeme, AbstactToken** out_token);

LexicalProcessor::LexicalProcessor()
{
    this->text = nullptr;
    this->last_token_position = 0;
    this->current_row = 0;
    this->current_column = 0;
}

LexicalProcessor::~LexicalProcessor()
{
    this->text = nullptr;
    this->last_token_position = 0;
    this->current_row = 0;
    this->current_column = 0;
}

void LexicalProcessor::bindText(const char* text)
{
    this->text = text;
    this->last_token_position = 0;
    this->current_row = 1;
    this->current_column = 1;
}

comp_error_t LexicalProcessor::getNextToken(AbstactToken** token)
{
    *token = nullptr;
    //лексема
    const char* lexeme = this->text + last_token_position;
    //позиция текущего рассматриваемого символа
    size_t symbol_index = 0;
    //возможные состояния автомата
    enum State
    {
        STATE_NULL,     //в начале лексемы
        STATE_INT,      //целое число
        STATE_REAL,     //вещественное число(были в целом числе и встретили точку)
        STATE_STRING,   //начинается с '
        STATE_IDENT,     //начинается с буквы
        STATE_LEXEMES_END//конец лексемы
    };
    //состояние автомата
    State state = STATE_NULL;
    //найденная ошибка разбора
    comp_error_t error;
    error.code = NO_ERRORS;

    LexemeType lexeme_type = LEXEME_UNKNOWN;
    //цикл разбора лексемы
    //здесь происходит только определение типа токена и определение его символьной длины
    while(state != STATE_LEXEMES_END)
    {
        //текущий считанный символ
        char symbol = lexeme[symbol_index];
        switch(state)
        {
            //находимся перед лексемой
            case STATE_NULL:
                {
                    if (symbol == '\0'){
                        state = STATE_LEXEMES_END;
                        error.code = LEX_ERROR_END_OF_FILE;
                        error.row = this->current_row;
                        error.col = this->current_column;
                    }
                    //если встретилась цифра или унарный минус или плюс
                    else if (isdigit(symbol) ||
                             ((symbol == '-' || symbol == '+') && isdigit(lexeme[symbol_index + 1])))
                        state = STATE_INT;
                    //если встретилась буква или нижнее подчеркивание
                    else if (isalpha(symbol) || symbol == '_')
                        state = STATE_IDENT;
                    //если встретилась двойная кавычка
                    else if (symbol == '\'')
                        state = STATE_STRING;
                    //если встретился служебный символ
                    else if (isServiceSymbol(symbol))
                    {
                        //разбор служебного символа
                        if ((symbol == ':' && lexeme[symbol_index + 1] == '=') ||
                            (symbol == '.' && lexeme[symbol_index + 1] == '.') ||
                            (symbol == '<' && lexeme[symbol_index + 1] == '>') ||
                            (symbol == '<' && lexeme[symbol_index + 1] == '=') ||
                            (symbol == '>' && lexeme[symbol_index + 1] == '='))
                        {
                            symbol_index++;
                        }
                        lexeme_type = LEXEME_IDENTIFIER;
                        state = STATE_LEXEMES_END;
                        symbol_index++;
                    }
                } break;
            //пытаемся разобрать число
            case STATE_INT:
                {
                    if (symbol == '.')
                        state = STATE_REAL;
                    else if (!isdigit(symbol)){
                        lexeme_type = LEXEME_INT_CONST;
                        state = STATE_LEXEMES_END;
                    }
                } break;
            //пытаемся разобрать вещественное число
            case STATE_REAL:
                {
                    if (!isdigit(symbol))
                    {
                        lexeme_type = LEXEME_REAL_CONST;
                        state = STATE_LEXEMES_END;
                    }
                } break;
            //пытаемся разобрать строковую константу
            case STATE_STRING:
                {
                    if (symbol == '\''){
                        lexeme_type = LEXEME_STRING_CONST;
                        state = STATE_LEXEMES_END;
                        symbol_index++;
                    }
                } break;
            //пытаемся разобрать идентификатор (ключевое слово или идентификатор пока не известно)
            case STATE_IDENT:
                {
                    if (isServiceSymbol(symbol) || isspace(symbol)){
                        lexeme_type = LEXEME_IDENTIFIER;
                        state = STATE_LEXEMES_END;
                    }
                } break;
            default: break;
        }
        this->current_column++;
        symbol_index++;
    }
    symbol_index--;
    //переводим лексему в std::string
    std::string value(lexeme, symbol_index);
    if (!error.code){
        error = ParseToken(lexeme_type, value, token);
        if (error.code)
        {
            error.col = this->current_column;
            error.row = this->current_row;
        }
    }
    //пропускаем пробельные символы
    while(isspace(lexeme[symbol_index])){
        if (lexeme[symbol_index] == '\n')
        {
            this->current_row++;
            this->current_column = 1;
        }
        symbol_index++;
    }
    this->last_token_position += symbol_index;
    return error;
}

comp_error_t ParseToken(LexemeType type, const std::string& lexeme, AbstactToken** out_token)
{
    AbstactToken* token = nullptr;
    comp_error_t err;
    err.code = NO_ERRORS;

    switch(type)
    {
    case LEXEME_IDENTIFIER:
        {
            PascalKeyword keyword = definePascalKeyword(lexeme);
            if (keyword == KEYWORD_UNKNOWN)
                token = new Token<std::string>(TOKEN_IDENTIFIER, lexeme);
            else token = new Token<PascalKeyword>(TOKEN_KEYWORD, keyword);
        } break;
    case LEXEME_INT_CONST:
        {
            int32_t value;
            try{
                value = std::stoi(lexeme);
            }
                catch(const std::out_of_range& e) {
                err.code = LEX_ERROR_OUT_OF_RANGE;
                break;
            }
            Constant<int32_t> *constant = new Constant<int32_t>(CONST_INT, value);
            token = new Token<Constant<int32_t>*>(TOKEN_CONST, constant);
        } break;
    case LEXEME_REAL_CONST:
        {
            float value = std::stod(lexeme);
            Constant<float> *constant = new Constant<float>(CONST_REAL, value);
            token = new Token<Constant<float>*>(TOKEN_CONST, constant);
        } break;
    case LEXEME_STRING_CONST:
        {
            std::string value = lexeme.substr(1, lexeme.size() - 2);
            if (value.size() == 1)
            {
                Constant<char>* constant = new Constant<char>(CONST_CHAR, value[0]);
                token = new Token<Constant<char>*>(TOKEN_CONST, constant);
            }
            else
            {
                Constant<std::string>* constant = new Constant<std::string>(CONST_STRING, value);
                token = new Token<Constant<std::string>*>(TOKEN_CONST, constant);
            }
        } break;
    default: {} break;
    }
    if (!err.code)
        *out_token = token;
    return err;
}
