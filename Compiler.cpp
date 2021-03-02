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
    errors.clear();
    last_token = nullptr;
}

void Compiler::bindReader(FileReader* reader)
{
    lexer.bindReader(reader);
}

ErrorsArray Compiler::Compile(void* const program)
{
    next_error = lexer.getNextToken(&next_token);
    //������� ������ ������
    errors.clear();
    this->compile_Program();
    return errors;
}

bool Compiler::readToken()
{
    if (last_token)
        delete last_token;
    last_token = next_token;
    bool result = true;
    if (next_error.code != NO_ERRORS){
        errors.push_back(next_error);
        result = false;
    }
    this->next_error = lexer.getNextToken(&next_token);
    return result;
}

bool Compiler::isNextTokenCorrect()
{
    return this->next_token && this->next_error.code == NO_ERRORS;
}

void Compiler::addError(ErrorType error_code)
{
    comp_error_t err;
    err.code = error_code;
    err.row = next_token->getRow();
    err.col = next_token->getColumn();
    errors.push_back(err);
}

void Compiler::SkipTokens(const StopWordsArray& stop_words, bool before_ident)
{
    while(!isNextTokenCorrect() ||
          //�� ���� �����
          !(next_token->getType() == TOKEN_KEYWORD && std::find(stop_words.begin(), stop_words.end(), ((Token<PascalKeyword>*)next_token)->getData()) != stop_words.end()) &&
          //� �� �������������
          !(before_ident && next_token->getType() == TOKEN_IDENTIFIER)
          )
    {
        readToken();
    }
}

bool Compiler::isNextTokenOperator()
{
    return this->isNextTokenCorrect() &&
                //���� �������������
                (next_token->getType() == TOKEN_IDENTIFIER ||
                //���� ��� for, while, repeat, goto, if, case, begin
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

// ----------------------- ���������� -----------------------------

bool Compiler::compile_Program()
{
    bool compiled = true;
    StopWordsArray block_begin_words = {KEYWORD_VAR, KEYWORD_TYPE,
                                        KEYWORD_FUNCTION, KEYWORD_PROCEDURE,
                                        KEYWORD_BEGIN};

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
                addError(SYN_ERROR_EXPECTED_SEMICOLON);
                SkipTokens(block_begin_words, false);
                compiled = false;
            }
        }
        else{
            addError(SYN_ERROR_EXPECTED_IDENTIFIER);
            SkipTokens(block_begin_words, false);
            compiled = false;
        }
    }
    else
    {
        addError(SYN_ERROR_EXPECTED_PROGRAM);
        SkipTokens(block_begin_words, false);
        compiled = false;
    }


    if (!this->compile_Block()){
        //���������� ������ �� ����� ���������
        SkipTokens({KEYWORD_DOT, KEYWORD_EOF}, false);
        compiled = false;
    }

    //��������� ����� � �����
    if (this->isNextTokenCorrect() && next_token->getType() == TOKEN_KEYWORD &&
        ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_DOT)
    {
        this->readToken();
    }
    else{
            addError(SYN_ERROR_EXPECTED_DOT);
            compiled = false;
    }
    return compiled;
}

bool Compiler::compile_Block()
{
    bool compiled = true;
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
        if (!this->compile_VarSection())
        {
            //���������� ������ �� ����� ������ ���������� - �� ������� ��� ��������� ��� ����� ����������
            SkipTokens({KEYWORD_BEGIN, KEYWORD_PROCEDURE, KEYWORD_FUNCTION}, false);
            compiled = false;
        }
    }



    if (!this->compile_OperatorSection())
    {
        compiled = false;
    }

    return compiled;
}

bool Compiler::compile_VarSection()
{
    bool compiled = true;
    StopWordsArray stop_words = {KEYWORD_SEMICOLON, KEYWORD_BEGIN, KEYWORD_EOF};
    //��������� var
    this->readToken();

    if (this->isNextTokenCorrect() && next_token->getType() == TOKEN_IDENTIFIER)
    {
        while (compiled && this->isNextTokenCorrect() && next_token->getType() == TOKEN_IDENTIFIER)
        {
            //����������� ������ ���������� ����������
            if (!this->compile_VarDeclaration())
                compiled = false;
        }
    }
    else
    {
        addError(SYN_ERROR_EXPECTED_IDENTIFIER);
        compiled = false;
    }

    return compiled;
}

bool Compiler::compile_VarDeclaration()
{
    bool compiled = true;
    //��������� ������ ����������
    this->readToken();
    //���� , <����������>
    while (this->isNextTokenCorrect() && next_token->getType() == TOKEN_KEYWORD &&
        ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_COMMA)
    {
        //��������� ,
        this->readToken();
        //��������� ����������
        if (this->isNextTokenCorrect() && next_token->getType() == TOKEN_IDENTIFIER)
        {
            this->readToken();
        }
        else
        {
            addError(SYN_ERROR_EXPECTED_IDENTIFIER);
            compiled = false;
        }
    }


    //��������� :
    if (this->isNextTokenCorrect() && next_token->getType() == TOKEN_KEYWORD &&
        ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_COLON)
    {
        this->readToken();
        //��������� ���
        if (this->isNextTokenCorrect() && next_token->getType() == TOKEN_IDENTIFIER)
        {
            this->readToken();
            //������� ;
             if (this->isNextTokenCorrect() && next_token->getType() == TOKEN_KEYWORD &&
                    ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_SEMICOLON)
            {
                this->readToken();
            }
            else{
                addError(SYN_ERROR_EXPECTED_SEMICOLON);
                compiled = false;
            }
        }
        else
        {
            addError(SYN_ERROR_EXPECTED_IDENTIFIER);
            compiled = false;
        }
    }
    else
    {
        addError(SYN_ERROR_EXPECTED_COLON);
        compiled = false;
    }

    return compiled;
}

bool Compiler::compile_OperatorSection()
{
    return this->compile_CompoundOperator();
}

bool Compiler::compile_CompoundOperator()
{
    bool compiled = true;
    //���������� ������ ���� �� ������� ���������, ���� �� ���������, �������, ���� �� ����� ���������
    StopWordsArray stop_words = {KEYWORD_END,
                                KEYWORD_BEGIN, KEYWORD_IF,
                                KEYWORD_CASE, KEYWORD_WHILE,
                                KEYWORD_REPEAT, KEYWORD_FOR,
                                KEYWORD_GOTO, KEYWORD_EOF};

    if (this->isNextTokenCorrect() && next_token->getType() == TOKEN_KEYWORD &&
        ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_BEGIN)
    {
        //������� begin
        this->readToken();
    }
    else
    {
        addError(SYN_ERROR_EXPECTED_BEGIN);
        compiled = false;
    }

    //����������� ���������
    while (isNextTokenOperator())
    {
        if (compile_Operator())
        {
            if (isNextTokenCorrect() && next_token->getType() == TOKEN_KEYWORD &&
                    ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_SEMICOLON)
            {
                readToken();
            }
            else{
               addError(SYN_ERROR_EXPECTED_SEMICOLON);
               compiled = false;
            }
        }
        else
        {
            SkipTokens(stop_words, true);
            compiled = false;
        }
    }

    //��������� end
    if (this->isNextTokenCorrect() && next_token->getType() == TOKEN_KEYWORD &&
        ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_END)
    {
        this->readToken();
    }
    else{
        addError(SYN_ERROR_EXPECTED_END);
        compiled = false;
    }

    return compiled;
}

bool Compiler::compile_Operator()
{
    bool compiled = true;
    if (isNextTokenCorrect())
    {
        //���� �������������(����������), �� ������������
        if (next_token->getType() == TOKEN_IDENTIFIER)
        {
            compiled = compile_Assignment();
        }
        //���� �������������(���������), �� ����� ���������
        //���� �������� ����� �� ���� ��� ������� ��� goto
        else if (next_token->getType() == TOKEN_KEYWORD)
        {
            Token<PascalKeyword>* token = (Token<PascalKeyword>*)next_token;
            switch(token->getData())
            {
                case KEYWORD_BEGIN: compiled = compile_CompoundOperator(); break;
                case KEYWORD_IF: compiled = compile_IfOperator(); break;
                case KEYWORD_FOR: break;
                case KEYWORD_CASE: break;
                case KEYWORD_WHILE: compiled = compile_WhileOperator(); break;
                case KEYWORD_REPEAT: break;
                default:
                    {
                        addError(SYN_ERROR_INVALID_OPERATOR);
                        compiled = false;
                    } break;
            }
        }
    }
    return compiled;
}

bool Compiler::compile_IfOperator()
{
    bool compiled = true;
    StopWordsArray stop_words = {KEYWORD_BEGIN, KEYWORD_END, KEYWORD_IF,
                                KEYWORD_CASE, KEYWORD_WHILE,
                                KEYWORD_REPEAT, KEYWORD_FOR,
                                KEYWORD_GOTO,
                                KEYWORD_THEN, KEYWORD_ELSE, KEYWORD_SEMICOLON};
    //read if
    readToken();
    if (!compile_Expression())
    {
        SkipTokens(stop_words, true);
        compiled = false;
    }

    if (this->isNextTokenCorrect() && next_token->getType() == TOKEN_KEYWORD &&
        ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_THEN)
    {
        //������� then
        this->readToken();
    }
    else
    {
        addError(SYN_ERROR_EXPECTED_THEN);
        SkipTokens(stop_words, true);
        compiled = false;
    }

    if (!compile_Operator())
    {
        SkipTokens(stop_words, true);
        compiled = false;
    }

    //���� ���� ����� �����
    if (this->isNextTokenCorrect() && next_token->getType() == TOKEN_KEYWORD &&
        ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_ELSE)
    {
        //������� else
        this->readToken();
        if (!compile_Operator())
        {
            SkipTokens(stop_words, true);
            compiled = false;
        }
    }
    return compiled;
}

bool Compiler::compile_WhileOperator()
{
    bool compiled = true;
    StopWordsArray stop_words = {KEYWORD_BEGIN, KEYWORD_END, KEYWORD_IF,
                                KEYWORD_CASE, KEYWORD_WHILE,
                                KEYWORD_REPEAT, KEYWORD_FOR,
                                KEYWORD_GOTO,
                                KEYWORD_DO};
    readToken();
    if (!compile_Expression())
    {
        SkipTokens(stop_words, false);
        compiled = false;
    }

    if (this->isNextTokenCorrect() && next_token->getType() == TOKEN_KEYWORD &&
        ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_DO)
    {
        //������� do
        this->readToken();
    }
    else
    {
        addError(SYN_ERROR_EXPECTED_DO);
        SkipTokens(stop_words, true);
        compiled = false;
    }

    if (!compile_Operator())
    {
        SkipTokens(stop_words, true);
        compiled = false;
    }
    return compiled;
}



bool Compiler::compile_Assignment()
{
    bool compiled = true;
    //read variable to assign
    readToken();
    //read :=
    if (this->isNextTokenCorrect() && next_token->getType() == TOKEN_KEYWORD &&
        ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_ASSIGNMENT)
    {
        readToken();
    }
    else
    {
        //���� ����� �� ����, �� ��������� ������, ������ ��� ����� ����� ���� � ���������� ������� ���������
        addError(SYN_ERROR_EXPECTED_ASSIGNMENT);
        compiled = false;
    }

    compiled = compile_Expression() && compiled;
    return compiled;
}

bool Compiler::compile_Expression()
{
    bool compiled = compile_SimpleExpression();
    if (isNextTokenCompareOperator())
    {
        readToken();
        compiled = compile_SimpleExpression() && compiled;
    }
    return compiled;
}

bool Compiler::compile_SimpleExpression()
{
    bool compiled = compile_Term();
    while (this->isNextTokenCorrect() && next_token->getType() == TOKEN_KEYWORD &&
            (
                ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_PLUS ||
                ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_MINUS ||
                ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_OR
            )
        )
    {
        readToken();
        compiled = compile_Term() && compiled;
    }

    return compiled;
}

bool Compiler::compile_Term()
{
    bool compiled = compile_Multiplier();
    while(this->isNextTokenCorrect() && next_token->getType() == TOKEN_KEYWORD &&
        (((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_ASTERISK ||
         ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_SLASH ||
         ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_DIV ||
         ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_MOD ||
         ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_AND)
        )
    {
        readToken();
        compiled = compile_Multiplier() && compiled;
    }
    return compiled;
}

bool Compiler::compile_Multiplier()
{
    bool compiled = true;
    //���� ����� ����������
    if (this->isNextTokenCorrect() && next_token->getType() == TOKEN_KEYWORD &&
        (((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_PLUS ||
         ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_MINUS)
        )
    {
        readToken();
    }


    if (isNextTokenCorrect())
    {
        //���������� ��� �������
        if (next_token->getType() == TOKEN_IDENTIFIER)
        {
            readToken();
        }
        //���������
        else if (next_token->getType() == TOKEN_CONST)
        {
            readToken();
        }
        //����������� ������
        else if(next_token->getType() == TOKEN_KEYWORD && ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_LEFT_PARENTHESIS)
        {
            readToken();
            compiled = compile_Expression();
            if (isNextTokenCorrect() && next_token->getType() == TOKEN_KEYWORD &&
                ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_RIGHT_PARENTHESIS)
            {
                readToken();
            }
            else {
                addError(SYN_ERROR_EXPECTED_RIGHT_PARENTHESIS);
                compiled = false;
            }
        }
    }
    else compiled = false;

    return compiled;
}

