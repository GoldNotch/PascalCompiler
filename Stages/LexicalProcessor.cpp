#include <cstdlib>
#include <cctype>
#include <algorithm>
#include "LexicalProcessor.h"
#include "../Types/PascalKeywords.h"
#include <stdio.h>

enum LexemeType
{
    LEXEME_UNKNOWN,
    LEXEME_IDENTIFIER,     //[A-Za-z_][\w_]*
    LEXEME_INT_CONST,   //\d+([.,]\d*)? 					-- неправильно, потому что еще бывают u, ull, ll, etc, а еще константа может начинаться с + и -
    LEXEME_REAL_CONST,
    LEXEME_STRING_CONST   //'any ascii symbol*'
  //[:=<>+\-*\/\(\)\{\}\[\]'^,;]+
};
comp_error_t ParseToken(LexemeType type,
                        size_t start_row,
                        size_t start_column,
                        const std::string& lexeme,
                        AbstactToken** out_token);

LexicalProcessor::LexicalProcessor()
{
    reader = nullptr;
}

LexicalProcessor::~LexicalProcessor()
{
    reader = nullptr;
}

void LexicalProcessor::bindReader(FileReader* reader)
{
    this->reader = reader;
}

comp_error_t LexicalProcessor::getNextToken(AbstactToken** token)
{
    *token = nullptr;
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
    std::string lexeme = "";
    size_t start_row = this->reader->getRowIndex();
    size_t start_column = this->reader->getColumnIndex();
    //цикл разбора лексемы
    //здесь происходит только определение типа токена и определение его символьной длины
    while(state != STATE_LEXEMES_END)
    {
        //текущий считанный символ
        char symbol = this->reader->readSymbol();
        char next_symbol = this->reader->peekSymbol();
        switch(state)
        {
            //находимся перед лексемой
            case STATE_NULL:
                {
                    //если встретилась цифра
                    if (isdigit(symbol))
                    {
                        lexeme.push_back(symbol);
                        if (next_symbol == '.')
                            state = STATE_REAL;
                        else if (!isdigit(next_symbol))
                        {
                            lexeme_type = LEXEME_INT_CONST;
                            state = STATE_LEXEMES_END;
                        }
                        else
                            state = STATE_INT;
                    }
                    //если встретилась буква или нижнее подчеркивание
                    else if (isalpha(symbol) || symbol == '_' || symbol == '\0'){
                        lexeme.push_back(symbol);
                        if (isServiceSymbol(next_symbol) || isspace(next_symbol))
                        {
                            lexeme_type = LEXEME_IDENTIFIER;
                            state = STATE_LEXEMES_END;
                        }
                        else
                        state = STATE_IDENT;
                    }
                    //если встретилась двойная кавычка
                    else if (symbol == '\''){
                        lexeme.push_back(symbol);
                        state = STATE_STRING;
                    }
                    //если встретился служебный символ
                    else if (isServiceSymbol(symbol))
                    {
                        lexeme.push_back(symbol);
                        //разбор служебного символа
                        if ((symbol == ':' && next_symbol == '=') ||
                            (symbol == '.' && next_symbol == '.') ||
                            (symbol == '<' && next_symbol == '>') ||
                            (symbol == '<' && next_symbol == '=') ||
                            (symbol == '>' && next_symbol == '='))
                        {
                            this->reader->readSymbol();
                            lexeme.push_back(next_symbol);
                        }
                        lexeme_type = LEXEME_IDENTIFIER;
                        state = STATE_LEXEMES_END;
                    }
                } break;
            //пытаемся разобрать число
            case STATE_INT:
                {
                    lexeme.push_back(symbol);
                    if (next_symbol == '.')
                        state = STATE_REAL;
                    else if (!isdigit(next_symbol)){
                        lexeme_type = LEXEME_INT_CONST;
                        state = STATE_LEXEMES_END;
                    }
                } break;
            //пытаемся разобрать вещественное число
            case STATE_REAL:
                {
                    lexeme.push_back(symbol);
                    if (!isdigit(next_symbol))
                    {
                        lexeme_type = LEXEME_REAL_CONST;
                        state = STATE_LEXEMES_END;
                    }
                } break;
            //пытаемся разобрать строковую константу
            case STATE_STRING:
                {
                    lexeme.push_back(symbol);
                    if (symbol == '\'')
                    {
                        lexeme_type = LEXEME_STRING_CONST;
                        state = STATE_LEXEMES_END;
                    }
                } break;
            //пытаемся разобрать идентификатор (ключевое слово или идентификатор пока не известно)
            case STATE_IDENT:
                {
                    lexeme.push_back(symbol);
                    if (isServiceSymbol(next_symbol) || isspace(next_symbol))
                    {
                        lexeme_type = LEXEME_IDENTIFIER;
                        state = STATE_LEXEMES_END;
                    }
                } break;
            default: break;
        }
    }
    if (!error.code){
        error = ParseToken(lexeme_type, start_row, start_column, lexeme, token);
        if (error.code)
        {
            error.col = this->reader->getColumnIndex();
            error.row = this->reader->getRowIndex();
        }
    }
    //пропускаем пробельные символы
    while(isspace(this->reader->peekSymbol()) && this->reader->peekSymbol() != '\n')
        this->reader->readSymbol();
    return error;
}

comp_error_t ParseToken(LexemeType type, size_t start_row, size_t start_column, const std::string& lexeme, AbstactToken** out_token)
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
                token = new Token<std::string>(TOKEN_IDENTIFIER, start_row, start_column, lexeme);
            else token = new Token<PascalKeyword>(TOKEN_KEYWORD, start_row, start_column, keyword);
        } break;
    case LEXEME_INT_CONST:
        {
            int32_t value;
            try{
                value = std::stoi(lexeme);
            }
            catch(...) {
                err.code = LEX_ERROR_OUT_OF_RANGE;
                err.row = start_row;
                err.col = start_column;
                break;
            }
            Constant<int32_t> *constant = new Constant<int32_t>(CONST_INT, value);
            token = new Token<Constant<int32_t>*>(TOKEN_CONST, start_row, start_column, constant);
        } break;
    case LEXEME_REAL_CONST:
        {
            float value = std::stod(lexeme);
            Constant<float> *constant = new Constant<float>(CONST_REAL, value);
            token = new Token<Constant<float>*>(TOKEN_CONST, start_row, start_column, constant);
        } break;
    case LEXEME_STRING_CONST:
        {
            std::string value = lexeme.substr(1, lexeme.size() - 2);
            if (value.size() == 1)
            {
                Constant<char>* constant = new Constant<char>(CONST_CHAR, value[0]);
                token = new Token<Constant<char>*>(TOKEN_CONST, start_row, start_column, constant);
            }
            else
            {
                Constant<std::string>* constant = new Constant<std::string>(CONST_STRING, value);
                token = new Token<Constant<std::string>*>(TOKEN_CONST, start_row, start_column, constant);
            }
        } break;
    default: {} break;
    }
    if (!err.code)
        *out_token = token;
    return err;
}
