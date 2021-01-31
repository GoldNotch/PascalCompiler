#pragma once
#include <cctype>
#include <cstring>
#include <map>
#include <string>

//перечисление всех ключевых слов языка паскаль
enum PascalKeyword
{
    KEYWORD_AND,        //and
    KEYWORD_ARRAY,      //array
    KEYWORD_BEGIN,      //begin
    KEYWORD_CASE,       //case
    KEYWORD_CONST,      //const
    KEYWORD_DIV,        //div
    KEYWORD_DO,         //do
    KEYWORD_DOWNTO,     //downto
    KEYWORD_ELSE,       //else
    KEYWORD_END,        //end
    KEYWORD_FILE,       //file
    KEYWORD_FOR,        //for
    KEYWORD_FUNCTION,   //function
    KEYWORD_GOTO,       //goto
    KEYWORD_IF,         //if
    KEYWORD_IN,         //in
    KEYWORD_LABEL,      //label
    KEYWORD_MOD,        //mod
    KEYWORD_NIL,        //nil
    KEYWORD_NOT,        //not
    KEYWORD_OF,         //of
    KEYWORD_OR,         //or
    KEYWORD_PACKED,     //packed
    KEYWORD_PROCEDURE,  //procedure
    KEYWORD_PROGRAM,    //program
    KEYWORD_RECORD,     //record
    KEYWORD_REPEAT,     //repeat
    KEYWORD_SET,        //set
    KEYWORD_THEN,       //then
    KEYWORD_TO,         //to
    KEYWORD_TYPE,       //type
    KEYWORD_UNTIL,      //until
    KEYWORD_VAR,        //var
    KEYWORD_WHILE,      //while
    KEYWORD_WITH,        //with
    KEYWORD_PLUS,                   //+
    KEYWORD_MINUS,                  //-
    KEYWORD_ASTERISK,               //*
    KEYWORD_SLASH,                  ///
    KEYWORD_EQUAL,                  //=
    KEYWORD_GREATER_THAN,           //>
    KEYWORD_LESS_THAN,              //<
    KEYWORD_LEFT_BRACKET,           //[
    KEYWORD_RIGHT_BRACKET,          //]
    KEYWORD_DOT,                    //.
    KEYWORD_COMMA,                  //,
    KEYWORD_COLON,                  //:
    KEYWORD_SEMICOLON,              //;
    KEYWORD_POINTER,                //^
    KEYWORD_LEFT_PARENTHESIS,       //(
    KEYWORD_RIGHT_PARENTHESIS,      //)
    KEYWORD_NON_EQUAL,              //<>
    KEYWORD_LESS_THAN_OR_EQUAL,     //<=
    KEYWORD_GREATER_THAN_OR_EQUAL,  //>=
    KEYWORD_ASSIGNMENT,             //:=
    KEYWORD_DOUBLE_DOT,             //..
    KEYWORD_TOTAL,
    KEYWORD_UNKNOWN,
};

static int isServiceSymbol(int symbol)
{
    static const char* symbols = "+-*/=<>()[]:;.,^";
    return strchr(symbols, symbol) != nullptr;
}

static PascalKeyword definePascalKeyword(std::string lexeme)
{
    static std::map<std::string, PascalKeyword> keywords {
        {"and" , KEYWORD_AND},
        {"array" , KEYWORD_ARRAY},
        {"begin" , KEYWORD_BEGIN},
        {"case" , KEYWORD_CASE},
        {"const" , KEYWORD_CONST},
        {"div" , KEYWORD_DIV},
        {"do" , KEYWORD_DO},
        {"downto" , KEYWORD_DOWNTO},
        {"else" , KEYWORD_ELSE},
        {"end" , KEYWORD_END},
        {"file" , KEYWORD_FILE},
        {"function" , KEYWORD_FUNCTION},
        {"goto" , KEYWORD_GOTO},
        {"if" , KEYWORD_IF},
        {"in" , KEYWORD_IN},
        {"label" , KEYWORD_LABEL},
        {"mod" , KEYWORD_MOD},
        {"nil" , KEYWORD_NIL},
        {"not" , KEYWORD_NOT},
        {"of" , KEYWORD_OF},
        {"or" , KEYWORD_OR},
        {"packed" , KEYWORD_PACKED},
        {"procedure" , KEYWORD_PROCEDURE},
        {"program" , KEYWORD_PROGRAM},
        {"record" , KEYWORD_RECORD},
        {"repeat" , KEYWORD_REPEAT},
        {"set" , KEYWORD_SET},
        {"then" , KEYWORD_THEN},
        {"to" , KEYWORD_TO},
        {"and" , KEYWORD_TYPE},
        {"type" , KEYWORD_UNTIL},
        {"var" , KEYWORD_VAR},
        {"while" , KEYWORD_WHILE},
        {"with" , KEYWORD_WITH},
        {"+" , KEYWORD_PLUS},
        {"-" , KEYWORD_MINUS},
        {"*" , KEYWORD_ASTERISK},
        {"/" , KEYWORD_SLASH},
        {"=" , KEYWORD_EQUAL},
        {">" , KEYWORD_GREATER_THAN},
        {"<" , KEYWORD_LESS_THAN},
        {"[" , KEYWORD_LEFT_BRACKET},
        {"]" , KEYWORD_RIGHT_BRACKET},
        {"." , KEYWORD_DOT},
        {"," , KEYWORD_COMMA},
        {":" , KEYWORD_COLON},
        {";" , KEYWORD_SEMICOLON},
        {"^" , KEYWORD_POINTER},
        {"(" , KEYWORD_LEFT_PARENTHESIS},
        {")" , KEYWORD_RIGHT_PARENTHESIS},
        {"<>" , KEYWORD_NON_EQUAL},
        {"<=" , KEYWORD_LESS_THAN_OR_EQUAL},
        {">=" , KEYWORD_GREATER_THAN_OR_EQUAL},
        {":=" , KEYWORD_ASSIGNMENT},
        {".." , KEYWORD_DOUBLE_DOT}
        };
    return keywords.find(lexeme) != keywords.end() ? keywords[lexeme] : KEYWORD_UNKNOWN;
}
