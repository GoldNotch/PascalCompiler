#include <algorithm>
#include "Compiler.h"
#include "Stages/LexicalProcessor.h"

typedef std::vector<PascalKeyword> StopWordsArray;

Compiler::Compiler()
{
    this->last_token = nullptr;
}

Compiler::~Compiler()
{
    if (last_token)
        delete last_token;
    last_token = nullptr;
}

void Compiler::bindReader(FileReader* reader)
{
    lexer.bindReader(reader);
}

ErrorsArray Compiler::Compile(void* const program)
{
    next_error = lexer.getNextToken(&next_token);
    return this->compile_Program();
}

comp_error_t Compiler::readToken()
{
    comp_error_t result;
    if (last_token)
        delete last_token;
    result = next_error;
    last_token = next_token;
    this->next_error = lexer.getNextToken(&next_token);
    return result;
}

bool Compiler::isNextTokenCorrect()
{
    return this->next_token && this->next_error.code == NO_ERRORS;
}

comp_error_t Compiler::createError(ErrorType error_code)
{
    comp_error_t err;
    err.code = error_code;
    err.row = next_token->getRow();
    err.col = next_token->getColumn();
    return err;
}

void Compiler::addError(ErrorType error_code, ErrorsArray* errors)
{
    errors->push_back(createError(error_code));
}

void Compiler::SkipTokens(const StopWordsArray& stop_words, bool before_ident)
{
    while(!isNextTokenCorrect() ||
          //не стоп слово
          !(next_token->getType() == TOKEN_KEYWORD && std::find(stop_words.begin(), stop_words.end(), ((Token<PascalKeyword>*)next_token)->getData()) != stop_words.end()) &&
          //и не идентификатор
          !(before_ident && next_token->getType() == TOKEN_IDENTIFIER)
          )
    {
        readToken();
    }
}

bool Compiler::isNextTokenOperator()
{
    return this->isNextTokenCorrect() &&
                //либо идентификатор
                (next_token->getType() == TOKEN_IDENTIFIER ||
                //либо это for, while, repeat, goto, if, case, begin
                (next_token->getType() == TOKEN_KEYWORD &&
                (((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_BEGIN ||
                 ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_FOR ||
                 ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_WHILE ||
                 ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_REPEAT ||
                 ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_GOTO ||
                 ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_IF ||
                 ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_CASE)
                 ));
}

bool Compiler::isNextTokenCompareOperator()
{
    return this->isNextTokenCorrect() && next_token->getType() == TOKEN_KEYWORD &&
                (
                 ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_GREATER_THAN ||
                 ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_GREATER_THAN_OR_EQUAL ||
                 ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_LESS_THAN ||
                 ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_LESS_THAN_OR_EQUAL ||
                 ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_EQUAL ||
                 ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_NON_EQUAL ||
                 ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_IN
                 );
}

// ----------------------- Грамматики -----------------------------

ErrorsArray Compiler::compile_Program()
{
    ErrorsArray errors;
    StopWordsArray stop_words = {KEYWORD_VAR, KEYWORD_TYPE,
                                KEYWORD_FUNCTION, KEYWORD_PROCEDURE,
                                KEYWORD_BEGIN, KEYWORD_DOT, KEYWORD_EOF};

    // read program
    if (this->isNextTokenCorrect() && next_token->getType() == TOKEN_KEYWORD &&
        ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_PROGRAM)
    {
        this->readToken();
        //read program name
        if (this->isNextTokenCorrect() && next_token->getType() == TOKEN_IDENTIFIER)
        {
            this->readToken();
            //read ;
            if (this->isNextTokenCorrect() && next_token->getType() == TOKEN_KEYWORD &&
                ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_SEMICOLON)
            {
                this->readToken();
            }
            else{
                addError(SYN_ERROR_EXPECTED_SEMICOLON, &errors);
                SkipTokens(stop_words, false);
            }
        }
        else{
            addError(SYN_ERROR_EXPECTED_IDENTIFIER, &errors);
            SkipTokens(stop_words, false);
        }
    }
    else
    {
        addError(SYN_ERROR_EXPECTED_PROGRAM, &errors);
        SkipTokens(stop_words, false);
    }


    ErrorsArray block_errors = this->compile_Block();
    if (block_errors.size() > 0){
        mergeErrorsArrays(&errors, block_errors);
        SkipTokens({KEYWORD_DOT, KEYWORD_EOF}, false);
    }
    //считываем точку в конце
    if (this->isNextTokenCorrect() && next_token->getType() == TOKEN_KEYWORD &&
        ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_DOT)
    {
        this->readToken();
    }
    else addError(SYN_ERROR_EXPECTED_DOT, &errors);
    return errors;
}

ErrorsArray Compiler::compile_Block()
{
    ErrorsArray errors;
    StopWordsArray stop_words = {KEYWORD_VAR, KEYWORD_PROCEDURE, KEYWORD_FUNCTION, KEYWORD_BEGIN};
    /*if (this->isNextTokenCorrect() && next_token->getType() == TOKEN_KEYWORD &&
        ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_TYPE)
    {
        errors = this->compile_TypeSection();
        if (errors.size() > 0)
        {
            SkipTokens(stop_words, false);
        }
    }*/

    if (this->isNextTokenCorrect() && next_token->getType() == TOKEN_KEYWORD &&
        ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_VAR)
    {
        ErrorsArray var_errors = this->compile_VarSection();
        if (var_errors.size() > 0)
        {
            mergeErrorsArrays(&errors, var_errors);
            SkipTokens(stop_words, false);
        }
    }

    //begin ... end.
    ErrorsArray operators_errors = this->compile_OperatorSection();
    mergeErrorsArrays(&errors, operators_errors);
    return errors;
}

ErrorsArray Compiler::compile_VarSection()
{
    ErrorsArray errors;
    StopWordsArray stop_words = {KEYWORD_SEMICOLON, KEYWORD_BEGIN, KEYWORD_EOF};
    //прочитали var
    this->readToken();

    if (this->isNextTokenCorrect() && next_token->getType() == TOKEN_IDENTIFIER)
    {
        while (this->isNextTokenCorrect() && next_token->getType() == TOKEN_IDENTIFIER)
        {
            //компилируем строку однотипных переменных
            comp_error_t error = this->compile_VarDeclaration();
            if (error.code == NO_ERRORS)
            {
                if (this->isNextTokenCorrect() && next_token->getType() == TOKEN_KEYWORD &&
                    ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_SEMICOLON)
                {
                    this->readToken();
                }
                else addError(SYN_ERROR_EXPECTED_SEMICOLON, &errors);
            }
            else{
                errors.push_back(error);
                //пропускаем до первого идентификатора или до функции, процедуры или основной программы
                SkipTokens({KEYWORD_PROCEDURE, KEYWORD_FUNCTION, KEYWORD_BEGIN}, true);
            }
        }
    }
    else addError(SYN_ERROR_EXPECTED_IDENTIFIER, &errors);

    return errors;
}

ErrorsArray Compiler::compile_OperatorSection()
{
    return this->compile_CompoundOperator();
}

ErrorsArray Compiler::compile_CompoundOperator()
{
    ErrorsArray errors;
    //пропускаем токены либо до первого оператора, либо до процедуры, функции, либо до конца программы
    StopWordsArray stop_words = {KEYWORD_END,
                                KEYWORD_BEGIN, KEYWORD_IF,
                                KEYWORD_CASE, KEYWORD_WHILE,
                                KEYWORD_REPEAT, KEYWORD_FOR,
                                KEYWORD_GOTO, KEYWORD_EOF};
    if (this->isNextTokenCorrect() && next_token->getType() == TOKEN_KEYWORD &&
        ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_BEGIN)
    {
        //считали begin
        this->readToken();
    }
    else
    {
        addError(SYN_ERROR_EXPECTED_BEGIN, &errors);
        SkipTokens(stop_words, true);
    }

    //компилируем операторы
    while (isNextTokenOperator())
    {
        ErrorsArray operator_errors = compile_Operator();
        if (operator_errors.size() > 0)
        {
            mergeErrorsArrays(&errors, operator_errors);
            SkipTokens({KEYWORD_SEMICOLON}, false);
            SkipTokens(stop_words, true);
        }
        else{
            if (isNextTokenCorrect() && next_token->getType() == TOKEN_KEYWORD &&
                    ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_SEMICOLON)
            {
                readToken();
            }
            else{
               addError(SYN_ERROR_EXPECTED_SEMICOLON, &errors);
               SkipTokens(stop_words, true);
            }
        }
    }

    //проверяем end
    if (this->isNextTokenCorrect() && next_token->getType() == TOKEN_KEYWORD &&
        ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_END)
    {
        this->readToken();
    }
    else addError(SYN_ERROR_EXPECTED_END, &errors);

    return errors;
}

ErrorsArray Compiler::compile_Operator()
{
    ErrorsArray errors;
    if (isNextTokenCorrect())
    {
        //если идентификатор(переменная), то присваивание
        if (next_token->getType() == TOKEN_IDENTIFIER)
        {
            comp_error_t err = compile_Assignment();
            if (err.code != NO_ERRORS)
                errors.push_back(err);
        }
        //если идентификатор(процедура), то вызов процедуры
        //если ключевое слово то цикл или условие или goto
        else if (next_token->getType() == TOKEN_KEYWORD)
        {
            Token<PascalKeyword>* token = (Token<PascalKeyword>*)next_token;
            switch(token->getData())
            {
                case KEYWORD_BEGIN: errors = compile_CompoundOperator(); break;
                case KEYWORD_IF: errors = compile_IfOperator(); break;
                case KEYWORD_FOR: break;
                case KEYWORD_CASE: break;
                case KEYWORD_WHILE: errors = compile_WhileOperator(); break;
                case KEYWORD_REPEAT: break;
                default:
                    {
                        addError(SYN_ERROR_INVALID_OPERATOR, &errors);
                    } break;
            }
        }
    }
    return errors;
}

ErrorsArray Compiler::compile_IfOperator()
{
    ErrorsArray errors;
    StopWordsArray stop_words = {KEYWORD_BEGIN, KEYWORD_END, KEYWORD_IF,
                                KEYWORD_CASE, KEYWORD_WHILE,
                                KEYWORD_REPEAT, KEYWORD_FOR,
                                KEYWORD_GOTO,
                                KEYWORD_THEN, KEYWORD_ELSE, KEYWORD_SEMICOLON};
    //read if
    readToken();
    comp_error_t err = compile_Expression();
    if (err.code != NO_ERRORS)
    {
        errors.push_back(err);
        SkipTokens(stop_words, true);
    }

    if (this->isNextTokenCorrect() && next_token->getType() == TOKEN_KEYWORD &&
        ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_THEN)
    {
        //считали then
        this->readToken();
    }
    else
    {
        addError(SYN_ERROR_EXPECTED_THEN, &errors);
        SkipTokens(stop_words, true);
    }

    ErrorsArray operator_errors = compile_Operator();
    if (operator_errors.size() > 0)
    {
        mergeErrorsArrays(&errors, operator_errors);
        SkipTokens(stop_words, true);
    }

    //если есть ветка иначе
    if (this->isNextTokenCorrect() && next_token->getType() == TOKEN_KEYWORD &&
        ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_ELSE)
    {
        //считали else
        this->readToken();
        ErrorsArray operator_errors = compile_Operator();
        if (operator_errors.size() > 0)
        {
            mergeErrorsArrays(&errors, operator_errors);
            SkipTokens(stop_words, false);
        }
    }
    return errors;
}

ErrorsArray Compiler::compile_WhileOperator()
{
    ErrorsArray errors;
    StopWordsArray stop_words = {KEYWORD_BEGIN, KEYWORD_END, KEYWORD_IF,
                                KEYWORD_CASE, KEYWORD_WHILE,
                                KEYWORD_REPEAT, KEYWORD_FOR,
                                KEYWORD_GOTO,
                                KEYWORD_DO};
    readToken();
    comp_error_t err = compile_Expression();
    if (err.code != NO_ERRORS)
    {
        errors.push_back(err);
        SkipTokens(stop_words, false);
    }

    if (this->isNextTokenCorrect() && next_token->getType() == TOKEN_KEYWORD &&
        ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_DO)
    {
        //считали do
        this->readToken();
    }
    else
    {
        addError(SYN_ERROR_EXPECTED_DO, &errors);
        SkipTokens(stop_words, true);
    }
    ErrorsArray operator_errors = compile_Operator();
    if (operator_errors.size() > 0)
    {
        mergeErrorsArrays(&errors, operator_errors);
        SkipTokens(stop_words, false);
    }
    return errors;
}


comp_error_t Compiler::compile_VarDeclaration()
{
    comp_error_t error;
    error.code = NO_ERRORS;
    //прочитали первую переменную
    this->readToken();
    //пока , <переменная>
    while (this->isNextTokenCorrect() && next_token->getType() == TOKEN_KEYWORD &&
        ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_COMMA)
    {
        //прочитали ,
        this->readToken();
        //прочитали переменную
        if (this->isNextTokenCorrect() && next_token->getType() == TOKEN_IDENTIFIER)
        {
            this->readToken();
        }
        else return createError(SYN_ERROR_EXPECTED_IDENTIFIER);
    }

    //считываем :
    if (this->isNextTokenCorrect() && next_token->getType() == TOKEN_KEYWORD &&
        ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_COLON)
    {
        this->readToken();
        //считываем тип
        if (this->isNextTokenCorrect() && next_token->getType() == TOKEN_IDENTIFIER)
        {
            this->readToken();
        }
        else return createError(SYN_ERROR_EXPECTED_IDENTIFIER);
    }
    else return createError(SYN_ERROR_EXPECTED_COLON);

    return error;
}

comp_error_t Compiler::compile_Assignment()
{
    //read variable to assign
    readToken();
    //read :=
    if (this->isNextTokenCorrect() && next_token->getType() == TOKEN_KEYWORD &&
        ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_ASSIGNMENT)
    {
        readToken();
        return compile_Expression();
    }
    else return createError(SYN_ERROR_EXPECTED_ASSIGNMENT);
}

comp_error_t Compiler::compile_Expression()
{
    comp_error_t err = compile_SimpleExpression();
    if (err.code == NO_ERRORS && isNextTokenCompareOperator())
    {
        readToken();
        err = compile_SimpleExpression();
    }
    return err;
}

comp_error_t Compiler::compile_SimpleExpression()
{
    comp_error_t error;
    if (this->isNextTokenCorrect() && next_token->getType() == TOKEN_KEYWORD &&
        (((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_PLUS ||
         ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_MINUS)
        )
    {
        readToken();
    }
    error = compile_Term();
    while (this->isNextTokenCorrect() && next_token->getType() == TOKEN_KEYWORD &&
            (
                ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_PLUS ||
                ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_MINUS ||
                ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_OR
            )
        )
    {
        readToken();
        error = compile_Term();
    }
    return error;
}

comp_error_t Compiler::compile_Term()
{
    comp_error_t error;
    error.code = NO_ERRORS;
    error = compile_Multiplier();
    while(this->isNextTokenCorrect() && next_token->getType() == TOKEN_KEYWORD &&
        (((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_ASTERISK ||
         ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_SLASH ||
         ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_DIV ||
         ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_MOD ||
         ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_AND)
        )
    {
        readToken();
        error = compile_Multiplier();
    }
    return error;
}

comp_error_t Compiler::compile_Multiplier()
{
    comp_error_t error;
    error.code = NO_ERRORS;
    if (isNextTokenCorrect())
    {
        //переменная или функция
        if (next_token->getType() == TOKEN_IDENTIFIER)
        {
            readToken();
        }
        else if (next_token->getType() == TOKEN_CONST)
        {
            readToken();
        }
        else if(next_token->getType() == TOKEN_KEYWORD && ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_LEFT_PARENTHESIS)
        {
            readToken();
            error = compile_Expression();
            if (isNextTokenCorrect() && next_token->getType() == TOKEN_KEYWORD &&
                ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_RIGHT_PARENTHESIS)
            {
                readToken();
            }
            else {
                error = createError(SYN_ERROR_EXPECTED_RIGHT_PARENTHESIS);
            }
        }
    }
    else error = next_error;
    return error;
}

