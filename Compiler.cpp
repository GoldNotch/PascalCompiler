#include <algorithm>
#include "Compiler.h"
#include "Stages/LexicalProcessor.h"

typedef std::vector<PascalKeyword> StopWordsArray;

//токена - начало оператора + идентификатор
static StopWordsArray OperatorBeginWords = {KEYWORD_BEGIN, KEYWORD_IF,
                                            KEYWORD_WHILE, KEYWORD_FOR,
                                            KEYWORD_REPEAT, KEYWORD_WITH,
                                            KEYWORD_GOTO, KEYWORD_CASE};
//токены - конец оператора
static StopWordsArray OperatorEndWords = {KEYWORD_END, KEYWORD_SEMICOLON, KEYWORD_EOF};
//токены - начало выражения + константа + идентификатор
static StopWordsArray ExpressionBeginWords = {KEYWORD_PLUS, KEYWORD_MINUS, KEYWORD_LEFT_PARENTHESIS};
//токены - конец выражения
static StopWordsArray ExpressionEndWords = {KEYWORD_END, KEYWORD_SEMICOLON, KEYWORD_EOF,
                                            KEYWORD_THEN, KEYWORD_DO, KEYWORD_OF,
                                            KEYWORD_TO, KEYWORD_DOWNTO};
static StopWordsArray BlockBeginWords = {KEYWORD_VAR, KEYWORD_TYPE,
                                        KEYWORD_FUNCTION, KEYWORD_PROCEDURE,
                                        KEYWORD_BEGIN};
static StopWordsArray ProgramEndWords = {KEYWORD_DOT, KEYWORD_EOF};

static std::map<PascalKeyword, int> operation_weight =    {
                                                            {KEYWORD_ASTERISK, 3},
                                                            {KEYWORD_SLASH, 3},
                                                            {KEYWORD_PLUS, 2},
                                                            {KEYWORD_MINUS, 2},
                                                            {KEYWORD_LEFT_PARENTHESIS, 0},
                                                            {KEYWORD_DIV, 3},
                                                            {KEYWORD_MOD, 3},
                                                            {KEYWORD_AND, 3},
                                                            {KEYWORD_OR, 2},
                                                            {KEYWORD_NOT, 4},
                                                            {KEYWORD_EQUAL, 1},
                                                            {KEYWORD_NON_EQUAL, 1},
                                                            {KEYWORD_LESS_THAN_OR_EQUAL, 1},
                                                            {KEYWORD_GREATER_THAN_OR_EQUAL, 1},
                                                            {KEYWORD_LESS_THAN, 1},
                                                            {KEYWORD_GREATER_THAN, 1},
                                                            {KEYWORD_IN, 1}
                                                        };

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

ErrorsArray Compiler::Compile(const char* result_path)
{
    next_error = lexer.getNextToken(&next_token);
    //очищаем список ошибок
    generator = new CodeGenerator(result_path);
    errors.clear();
    global_scope = new Scope();
    global_scope->SetTypeName("Integer", global_scope->createType(TYPE_SCALAR_4B));
    global_scope->SetTypeName("Real", global_scope->createType(TYPE_REAL));
    auto boolean = global_scope->createType(TYPE_SCALAR_1B);
    global_scope->SetTypeName("Boolean", boolean);
    auto character = global_scope->createType(TYPE_SCALAR_1B);
    global_scope->SetTypeName("Char", character);
    global_scope->addVariable("True", boolean, true)->setValue(1);
    global_scope->addVariable("False", boolean, true)->setValue(0);
    auto function = global_scope->createType(TYPE_FUNCTION);
    //у input значение = 1, у output значение = 0
    global_scope->addVariable("readln", function, true)->setValue(1);
    global_scope->addVariable("writeln", function, true)->setValue(0);
    generator->generateScope(global_scope);
    //заполнить область видимости стандартными типами и константами
    this->compile_Program();
    delete global_scope;
    delete generator;
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

void Compiler::SkipTokens(const StopWordsArray& stop_words, bool before_ident, bool before_const)
{
    while(!isNextTokenCorrect() || (
          //не стоп слово
          !(next_token->getType() == TOKEN_KEYWORD && std::find(stop_words.begin(), stop_words.end(), ((Token<PascalKeyword>*)next_token)->getData()) != stop_words.end()) &&
          //и не идентификатор
          !(before_ident && next_token->getType() == TOKEN_IDENTIFIER) &&
          //и не константа
          !(before_const && next_token->getType() == TOKEN_CONST)
          )
    )
    {
        readToken();
    }
}

bool Compiler::isNextTokenOperator()
{
    return this->isNextTokenCorrect() &&
            (
                //либо идентификатор
                next_token->getType() == TOKEN_IDENTIFIER ||
                //либо одно из ключевых слов
                (next_token->getType() == TOKEN_KEYWORD &&
                    std::find(OperatorBeginWords.begin(), OperatorBeginWords.end(), ((Token<PascalKeyword>*)next_token)->getData()) != OperatorBeginWords.end())
            );
}

bool Compiler::isNextTokenType()
{
    if (isNextTokenCorrect())
    {
        if (next_token->getType() == TOKEN_IDENTIFIER)
        {
            std::string type_name = ((Token<Identifier>*)next_token)->getData();
            return global_scope->getTypeById(type_name) != nullptr;
        }
        else return next_token->getType() == TOKEN_CONST ||
                    (next_token->getType() == TOKEN_KEYWORD && ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_RECORD) ||
                    (next_token->getType() == TOKEN_KEYWORD && ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_ARRAY);
    }
    else return false;
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

bool Compiler::isNextTokenVariable()
{
    if (isNextTokenCorrect() && next_token->getType() == TOKEN_IDENTIFIER)
    {
        std::string var_name = ((Token<Identifier>*)next_token)->getData();
        return global_scope->getDataById(var_name) != nullptr;
    }
    else return false;
}

bool Compiler::isAssignableTypes(AbstractType* t1, AbstractType* t2)
{
    return t1 != nullptr && t2 != nullptr &&
            (t1->getType() == t2->getType() ||
            (t1->getType() == TYPE_SCALAR_4B && t2->getType() == TYPE_REAL) ||
            (t1->getType() == TYPE_REAL && t2->getType() == TYPE_SCALAR_4B));
}

// ----------------------- Грамматики -----------------------------

bool Compiler::compile_Program()
{
    bool compiled = true;
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
                SkipTokens(BlockBeginWords, false, false);
                    compiled = false;
                }

        }
        else{
            addError(SYN_ERROR_EXPECTED_IDENTIFIER);
            SkipTokens(BlockBeginWords, false, false);
            compiled = false;
        }
    }
    else
    {
        addError(SYN_ERROR_EXPECTED_PROGRAM);
        SkipTokens(BlockBeginWords, false, false);
        compiled = false;
    }

    if (!this->compile_Block()){
        //пропускаем токены до конца программы
        //SkipTokens(ProgramEndWords, false, false);
        compiled = false;
    }

    //считываем точку в конце
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
    Scope* parent_scope = global_scope;
    global_scope = new Scope(global_scope);

    if (this->isNextTokenCorrect() && next_token->getType() == TOKEN_KEYWORD &&
        ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_TYPE)
    {
        if (!this->compile_TypeSection())
        {
            //пропускаем токены до конца секции переменных - до функции или процедуры или блока операторов
            SkipTokens({KEYWORD_VAR, KEYWORD_BEGIN, KEYWORD_PROCEDURE, KEYWORD_FUNCTION}, false, false);
            compiled = false;
        }
    }

    if (this->isNextTokenCorrect() && next_token->getType() == TOKEN_KEYWORD &&
        ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_VAR)
    {
        if (!this->compile_VarSection())
        {
            //пропускаем токены до конца секции переменных - до функции или процедуры или блока операторов
            SkipTokens({KEYWORD_BEGIN, KEYWORD_PROCEDURE, KEYWORD_FUNCTION}, false, false);
            compiled = false;
        }
    }

    generator->generateScope(global_scope);
    //global_scope->print();

    if (!this->compile_OperatorSection())
    {
        compiled = false;
    }

    delete global_scope;
    global_scope = parent_scope;
    return compiled;
}

bool Compiler::compile_VarSection()
{
    bool compiled = true;
    StopWordsArray stop_words = {KEYWORD_SEMICOLON, KEYWORD_BEGIN, KEYWORD_EOF};
    //прочитали var
    this->readToken();

    if (this->isNextTokenCorrect() && next_token->getType() == TOKEN_IDENTIFIER)
    {
        while (compiled && this->isNextTokenCorrect() && next_token->getType() == TOKEN_IDENTIFIER)
        {
            //компилируем строку однотипных переменных
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
    //the same type variables
    std::vector<Identifier> variables;
    //read first var_name
    this->readToken();
    Identifier var_name = ((Token<std::string>*)last_token)->getData();
    variables.push_back(var_name);
    //прочитали первую переменную
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
            var_name = ((Token<Identifier>*)last_token)->getData();
            variables.push_back(var_name);
        }
        else
        {
            addError(SYN_ERROR_EXPECTED_IDENTIFIER);
            compiled = false;
        }
    }


    //считываем :
    if (this->isNextTokenCorrect() && next_token->getType() == TOKEN_KEYWORD &&
        ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_COLON)
    {
        //read :
        this->readToken();
        //считываем тип
        if (isNextTokenType())
        {
            compiled = compile_Type() && compiled;
            //добавляем переменные в область видимости
            if (last_compiled_type)
            {
                for(size_t i = 0; i < variables.size(); i++)
                    global_scope->addVariable(variables[i], last_compiled_type, false);
            }
            last_compiled_type = nullptr;
            //считали ;
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

bool Compiler::compile_Type()
{
    bool compiled = true;
    if (isNextTokenCorrect()){
        if (next_token->getType() == TOKEN_IDENTIFIER)
        {
            readToken();
            Identifier type_name = ((Token<std::string>*)last_token)->getData();
            last_compiled_type = global_scope->getTypeById(type_name);
        }
        else if (next_token->getType() == TOKEN_CONST)
        {
            readToken();
            //считали ..
            if (this->isNextTokenCorrect() && next_token->getType() == TOKEN_KEYWORD &&
                    ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_DOUBLE_DOT)
            {
                this->readToken();
            }
            else{
                addError(SYN_ERROR_EXPECTED_DOUBLE_DOT);
                compiled = false;
            }
            if (this->isNextTokenCorrect() && next_token->getType() == TOKEN_CONST)
                readToken();
            else{
                addError(SYN_ERROR_EXPECTED_CONST);
                compiled = false;
            }
        }
        else if (next_token->getType() == TOKEN_KEYWORD && ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_RECORD)
        {
            compiled = compile_Record() && compiled;
        }
    }
    else {
        addError(SYN_ERROR_INVALID_TYPE);
        compiled = false;
    }
    return compiled;
}

bool Compiler::compile_OperatorSection()
{
    if (this->isNextTokenCorrect() && next_token->getType() == TOKEN_KEYWORD &&
        ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_BEGIN)
    {
        return this->compile_CompoundOperator();
    }
    else
    {
        addError(SYN_ERROR_EXPECTED_BEGIN);
        return false;
    }
}

bool Compiler::compile_CompoundOperator()
{
    bool compiled = true;
    //считали begin
    this->readToken();

    //компилируем операторы
    while (isNextTokenOperator())
    {
        if (!compile_Operator())
        {
            SkipTokens(OperatorEndWords, false, false);
            compiled = false;
        }
        if (isNextTokenCorrect() && next_token->getType() == TOKEN_KEYWORD &&
                    ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_SEMICOLON)
        {
            readToken();
        }
        else{
            //если нет точки запятой то пропускаем до след. оператора или до end
            StopWordsArray stopwords;
            stopwords.insert(stopwords.end(), OperatorBeginWords.begin(), OperatorBeginWords.end());
            stopwords.push_back(KEYWORD_END);
            addError(SYN_ERROR_EXPECTED_SEMICOLON);
            SkipTokens(stopwords, true, false);
            compiled = false;
        }
    }


    //проверяем end
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
        //если идентификатор(переменная), то присваивание
        if (next_token->getType() == TOKEN_IDENTIFIER)
        {
            std::string var_name = ((Token<std::string>*)next_token)->getData();
            Data* var = global_scope->getDataById(var_name);
            if (var->getType()->getType() == TYPE_FUNCTION)
                compiled = compile_FunctionCall();
            else
                compiled = compile_Assignment();
        }
        //если идентификатор(процедура), то вызов процедуры
        //если ключевое слово то цикл или условие или goto
        else if (next_token->getType() == TOKEN_KEYWORD)
        {
            Token<PascalKeyword>* token = (Token<PascalKeyword>*)next_token;
            switch(token->getData())
            {
                case KEYWORD_BEGIN: compiled = compile_CompoundOperator(); break;
                case KEYWORD_IF: compiled = compile_IfOperator(); break;
                case KEYWORD_FOR: compiled = compile_ForOperator(); break;
                case KEYWORD_CASE: break;
                case KEYWORD_WHILE: compiled = compile_WhileOperator(); break;
                case KEYWORD_REPEAT: compiled = compile_RepeatOperator(); break;
                case KEYWORD_WITH: compiled = compile_WithOperator(); break;
                default:
                    {
                        addError(SYN_ERROR_INVALID_OPERATOR);
                        compiled = false;
                    } break;
            }
        }
        else
        {
            addError(SYN_ERROR_INVALID_OPERATOR);
            compiled = false;
        }
    }
    return compiled;
}

bool Compiler::compile_IfOperator()
{
    bool compiled = true;
    std::string continue_label = "continue";
    std::string check_condition_label = "check_cond";
    generator->ReserveLabel(continue_label);
    generator->ReserveLabel(check_condition_label);
    //read if
    readToken();
    if (!compile_Expression())
    {
        SkipTokens({KEYWORD_THEN, KEYWORD_EOF}, false, false);
        compiled = false;
    }
    generator->Jump(check_condition_label);
    int then_label = generator->createLabel();

    if (this->isNextTokenCorrect() && next_token->getType() == TOKEN_KEYWORD &&
        ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_THEN)
    {
        //считали then
        this->readToken();
    }
    else
    {
        addError(SYN_ERROR_EXPECTED_THEN);
        compiled = false;
    }

    StopWordsArray stop_words;
    stop_words.insert(stop_words.end(), OperatorEndWords.begin(), OperatorEndWords.end());
    stop_words.push_back(KEYWORD_ELSE);
    if (!compile_Operator())
    {
        SkipTokens(stop_words, false, false);
        compiled = false;
    }
    generator->Jump(continue_label);

    //если есть ветка иначе
    int else_label = 0;
    if (this->isNextTokenCorrect() && next_token->getType() == TOKEN_KEYWORD &&
        ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_ELSE)
    {
        //считали else
        this->readToken();
        else_label = generator->createLabel();
        if (!compile_Operator())
        {
            SkipTokens(OperatorEndWords, false, false);
            compiled = false;
        }
        generator->Jump(continue_label);
    }
    generator->createLabel(check_condition_label);
    generator->JumpWithCondition(then_label, else_label);
    generator->createLabel(continue_label);
    return compiled;
}

bool Compiler::compile_WhileOperator()
{
    bool compiled = true;
    readToken();
    std::string begin_label = "begin_loop";
    std::string check_condition_label = "check_cond";
    generator->ReserveLabel(begin_label);
    generator->ReserveLabel(check_condition_label);
    generator->createLabel(begin_label);

    if (!compile_Expression())
    {
        SkipTokens({KEYWORD_DO}, false, false);
        compiled = false;
    }
    generator->Jump(check_condition_label);

    if (this->isNextTokenCorrect() && next_token->getType() == TOKEN_KEYWORD &&
        ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_DO)
    {
        //считали do
        this->readToken();
    }
    else
    {
        addError(SYN_ERROR_EXPECTED_DO);
        compiled = false;
    }
    int do_label = generator->createLabel();
    if (!compile_Operator())
    {
        SkipTokens(OperatorEndWords, false, false);
        compiled = false;
    }
    generator->Jump(begin_label);

    generator->createLabel(check_condition_label);
    generator->JumpWithCondition(do_label, 0);

    return compiled;
}

bool Compiler::compile_Variable()
{
    bool compiled = true;
    //read variable name;
    readToken();
    std::string var_name = ((Token<std::string>*)last_token)->getData();
    last_compiled_variable = global_scope->getDataById(var_name);
    //read .<member>
    if (last_compiled_variable->getType()->getType() == TYPE_RECORD &&
        this->isNextTokenCorrect() && next_token->getType() == TOKEN_KEYWORD &&
        ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_DOT)
    {
        //read .
        readToken();
        Scope* parent_scope = global_scope;
        global_scope = (Scope*)((RecordType*)last_compiled_variable->getType())->getScope();
        if (isNextTokenVariable()){
            compiled = compile_Variable() && compiled;
        }
        else
        {
            addError(SEM_ERROR_UNKNOWN_IDENT);
            compiled = false;
        }
        global_scope = parent_scope;
    }
    return compiled;
}

bool Compiler::compile_FunctionCall()
{
    bool compiled = true;
    compiled = compile_Variable();
    bool is_input = last_compiled_variable->getValue();
    //read (
    if (this->isNextTokenCorrect() && next_token->getType() == TOKEN_KEYWORD &&
        ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_LEFT_PARENTHESIS)
    {
        readToken();
    }
    else
    {
        //Если равно не было, то вставляем ошибку, делаем вид будто равно было и продолжаем парсинг выражения
        addError(SYN_ERROR_EXPECTED_LEFT_PARENTHESIS);
        compiled = false;
    }

    std::vector<Data*> variables;
    if (this->isNextTokenCorrect() && next_token->getType() == TOKEN_IDENTIFIER)
    {
        compiled = compile_Variable() && compiled;
        variables.push_back(last_compiled_variable);
    }
    while(this->isNextTokenCorrect() && next_token->getType() == TOKEN_KEYWORD &&
          ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_COMMA)
    {
        readToken();
        compiled = compile_Variable() && compiled;
        variables.push_back(last_compiled_variable);
    }

    //read (
    if (this->isNextTokenCorrect() && next_token->getType() == TOKEN_KEYWORD &&
        ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_RIGHT_PARENTHESIS)
    {
        readToken();
    }
    else
    {
        //Если равно не было, то вставляем ошибку, делаем вид будто равно было и продолжаем парсинг выражения
        addError(SYN_ERROR_EXPECTED_RIGHT_PARENTHESIS);
        compiled = false;
    }

    if (compiled)
    {
        if (is_input)
            generator->InputIntVariables(variables);
        else
            generator->OutputIntVariables(variables);
    }

    return compiled;
}

bool Compiler::compile_Assignment()
{
    bool compiled = true;
    AbstractType* first_type = nullptr;
    std::string variable_to_assign;
    if (isNextTokenVariable())
    {
         compiled = compile_Variable();
         first_type = last_compiled_variable->getType();
         variable_to_assign = last_compiled_variable->getName();
    }
    else
    {
        addError(SEM_ERROR_UNKNOWN_IDENT);
        SkipTokens({KEYWORD_ASSIGNMENT}, false, false);
        compiled = false;
    }


    //read :=
    if (this->isNextTokenCorrect() && next_token->getType() == TOKEN_KEYWORD &&
        ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_ASSIGNMENT)
    {
        readToken();
    }
    else
    {
        //Если равно не было, то вставляем ошибку, делаем вид будто равно было и продолжаем парсинг выражения
        addError(SYN_ERROR_EXPECTED_ASSIGNMENT);
        SkipTokens(ExpressionBeginWords, true, true);
        compiled = false;
    }

    if (!compile_Expression())
    {
        SkipTokens(ExpressionEndWords, false, false);
        compiled = false;
    }
    if (compiled && !isAssignableTypes(last_compiled_type, first_type))
    {
        addError(SEM_ERROR_NOT_ASSIGNABLE_TYPES);
        compiled = false;
    }
    if (compiled)
        generator->SaveToVariable(variable_to_assign);
    return compiled;
}

bool Compiler::compile_Expression()
{
    bool compiled = true;

    compiled = compile_SimpleExpression();
    if (isNextTokenCompareOperator())
    {
        readToken();
        PascalKeyword operation = ((Token<PascalKeyword>*)last_token)->getData();
        while(!opstack.empty() && operation_weight[operation] <= operation_weight[opstack.top()])
        {
            if (compiled)
                generator->ExecuteBinaryOperation(opstack.top());
            opstack.pop();
        }
        opstack.push(operation);
        compiled = compile_SimpleExpression() && compiled;
        last_compiled_type = global_scope->getTypeById("boolean");
    }
    while(!opstack.empty() && opstack.top() != KEYWORD_LEFT_PARENTHESIS)
    {
        if (compiled)
            generator->ExecuteBinaryOperation(opstack.top());
        opstack.pop();
    }
    return compiled;
}

bool Compiler::compile_SimpleExpression()
{
    bool compiled = compile_Term();
    AbstractType* first_type = last_compiled_type;
    //здесь проверку на соотношение типов
    while (this->isNextTokenCorrect() && next_token->getType() == TOKEN_KEYWORD &&
            (
                ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_PLUS ||
                ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_MINUS ||
                ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_OR
            )
        )
    {
        readToken();
        PascalKeyword operation = ((Token<PascalKeyword>*)last_token)->getData();
        while(!opstack.empty() && operation_weight[operation] <= operation_weight[opstack.top()])
        {
            if (compiled)
            generator->ExecuteBinaryOperation(opstack.top());
            opstack.pop();
        }
        opstack.push(operation);

        compiled = compile_Term() && compiled;
        if (compiled && !isAssignableTypes(last_compiled_type, first_type))
        {
            addError(SEM_ERROR_NOT_ASSIGNABLE_TYPES);
            compiled = false;
        }
        else first_type = last_compiled_type;
    }

    return compiled;
}

bool Compiler::compile_Term()
{
    bool compiled = compile_Multiplier();
    AbstractType* first_type = last_compiled_type;
    //здесь проверку на соотношение типов
    while(this->isNextTokenCorrect() && next_token->getType() == TOKEN_KEYWORD &&
        (((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_ASTERISK ||
         ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_SLASH ||
         ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_DIV ||
         ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_MOD ||
         ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_AND)
        )
    {
        readToken();
        PascalKeyword operation = ((Token<PascalKeyword>*)last_token)->getData();
        while(!opstack.empty() && operation_weight[operation] <= operation_weight[opstack.top()])
        {
            if (compiled)
                generator->ExecuteBinaryOperation(opstack.top());
            opstack.pop();
        }
        opstack.push(operation);

        compiled = compile_Multiplier() && compiled;
        if (compiled && !isAssignableTypes(last_compiled_type, first_type))
        {
            addError(SEM_ERROR_NOT_ASSIGNABLE_TYPES);
            compiled = false;
        }
        else first_type = last_compiled_type;
    }
    return compiled;
}

bool Compiler::compile_Multiplier()
{
    bool compiled = true;
    bool is_negate = false;
    //знак перед множителем
    if (this->isNextTokenCorrect() && next_token->getType() == TOKEN_KEYWORD &&
        (((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_PLUS ||
         ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_MINUS)
        )
    {
        readToken();
        if (((Token<PascalKeyword>*)last_token)->getData() == KEYWORD_MINUS)
            is_negate = true;
    }

    if (isNextTokenCorrect())
    {
        //переменная
        if (next_token->getType() == TOKEN_IDENTIFIER)
        {
            if (isNextTokenVariable()){
                compiled = compile_Variable() && compiled;
                last_compiled_type = last_compiled_variable->getType();
                if (compiled)
                    generator->pushVariable(last_compiled_variable->getName(), is_negate);
                last_compiled_variable = nullptr;
            }
            else
            {
                addError(SEM_ERROR_UNKNOWN_IDENT);
                compiled = false;
            }
        }
        //константа
        else if (next_token->getType() == TOKEN_CONST)
        {
            readToken();
            AbstactConstant* c = ((Token<AbstactConstant*>*)last_token)->getData();
            switch(c->getType())
            {
                case CONST_INT:
                    {
                        last_compiled_type = global_scope->getTypeById("integer");
                        if (is_negate)
                            generator->pushValue(-((Constant<int>*)c)->getValue());
                        else
                            generator->pushValue(((Constant<int>*)c)->getValue());
                    }  break;
                case CONST_REAL: last_compiled_type = global_scope->getTypeById("real"); break;
                case CONST_CHAR: last_compiled_type = global_scope->getTypeById("char"); break;
                default: last_compiled_type = nullptr; break;
            }
        }
        //открывающая скобка
        else if(next_token->getType() == TOKEN_KEYWORD && ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_LEFT_PARENTHESIS)
        {
            opstack.push(KEYWORD_LEFT_PARENTHESIS);
            readToken();
            compiled = compile_Expression();
            if (isNextTokenCorrect() && next_token->getType() == TOKEN_KEYWORD &&
                ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_RIGHT_PARENTHESIS)
            {
                readToken();
                while(opstack.top() != KEYWORD_LEFT_PARENTHESIS)
                {
                    generator->ExecuteBinaryOperation(opstack.top());
                    opstack.pop();
                }
                opstack.pop();
            }
            else {
                addError(SYN_ERROR_EXPECTED_RIGHT_PARENTHESIS);
                compiled = false;
            }
        }
        else
        {
            addError(SYN_ERROR_EXPECTED_MULTIPLIER);
            compiled = false;
        }
    }
    else{
        compiled = false;
        last_compiled_type = nullptr;
    }

    return compiled;
}

// ---------------- Индивидуальное задание --------------------------

bool Compiler::compile_TypeSection()
{
    bool compiled = true;
    //read type
    this->readToken();
    //read type declaration
    while (this->isNextTokenCorrect() && next_token->getType() == TOKEN_IDENTIFIER)
    {
        compiled = this->compile_TypeDeclatation() && compiled;
        if (isNextTokenCorrect() && next_token->getType() == TOKEN_KEYWORD &&
                ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_SEMICOLON)
        {
            readToken();
        }
        else {
            addError(SYN_ERROR_EXPECTED_SEMICOLON);
            compiled = false;
        }
    }
    return compiled;
}

bool Compiler::compile_TypeDeclatation()
{
    bool compiled = true;
    //read type_name
    this->readToken();
    std::string type_name = ((Token<Identifier>*)last_token)->getData();

    if (isNextTokenCorrect() && next_token->getType() == TOKEN_KEYWORD &&
            ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_EQUAL)
    {
        readToken();
    }
    else {
        addError(SYN_ERROR_EXPECTED_EQUAL);
        compiled = false;
    }
    compiled = compile_Type() && compiled;
    global_scope->SetTypeName(type_name, last_compiled_type);
    return compiled;
}

bool Compiler::compile_Record()
{
    bool compiled = true;
    //read record
    readToken();
    //create scope for structure
    Scope* parent_scope = global_scope;
    RecordType* new_type = (RecordType*)global_scope->createType(TYPE_RECORD);
    global_scope = (Scope*)new_type->getScope();
    while (compiled && this->isNextTokenCorrect() && next_token->getType() == TOKEN_IDENTIFIER)
    {
        //компилируем строку однотипных переменных
        if (!this->compile_VarDeclaration())
            compiled = false;
    }

    if (isNextTokenCorrect() && next_token->getType() == TOKEN_KEYWORD &&
            ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_END)
    {
        readToken();
    }
    else {
        addError(SYN_ERROR_EXPECTED_END);
        compiled = false;
    }
    global_scope = parent_scope;
    last_compiled_type = new_type;
    return compiled;
}

bool Compiler::compile_WithOperator()
{
    bool compiled = true;
    //read with
    readToken();
    std::vector<std::string> records;
    if (isNextTokenCorrect() && next_token->getType() == TOKEN_IDENTIFIER)
    {
        readToken();
        std::string var_name = ((Token<std::string>*)last_token)->getData();
        records.push_back(var_name);
    }
    else
    {
        addError(SYN_ERROR_EXPECTED_IDENTIFIER);
        compiled = false;
    }

    while (isNextTokenCorrect() && next_token->getType() == TOKEN_KEYWORD &&
        ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_COMMA)
    {
        readToken();
        if (isNextTokenCorrect() && next_token->getType() == TOKEN_IDENTIFIER)
        {
            readToken();
            std::string var_name = ((Token<std::string>*)last_token)->getData();
            records.push_back(var_name);
        }
        else
        {
            addError(SYN_ERROR_EXPECTED_IDENTIFIER);
            compiled = false;
        }
    }


    std::string label;
    if (isNextTokenCorrect() && next_token->getType() == TOKEN_KEYWORD &&
            ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_DO)
    {
        label = "with_" + std::to_string(next_token->getRow()) + std::to_string(next_token->getColumn());
        //запоминаем позицию токена
        lexer.SaveTokenPosition(label);
        readToken();
    }
    else {
        addError(SYN_ERROR_EXPECTED_DO);
        compiled = false;
    }

    for(size_t i = 0; i < records.size(); i++)
    {
        std::string& var_name = records[i];
        Scope *parent = global_scope;
        RecordType* record = (RecordType*)global_scope->getDataById(var_name)->getType();
        global_scope = (Scope*)record->getScope();
        printf("%s\n", next_token->toString().c_str());
        //компилируем оператор с новой областью видимости
        compiled = this->compile_Operator() && compiled;
        global_scope = parent;
        if (records.size() > 1 && i < records.size() - 1)
        {
            //возвращаемся к началу оператора
            lexer.RollbackToToken(label);
            this->readToken();
        }
    }

    //удаляем сохраненную метку токена
    lexer.PopSavedTokenPosition(label);
    return compiled;
}

bool Compiler::compile_ForOperator()
{
    bool compiled = true;
    //read for
    readToken();
    //read var
    if (isNextTokenCorrect() && next_token->getType() == TOKEN_IDENTIFIER)
    {
        readToken();
    }
    else {
        addError(SYN_ERROR_EXPECTED_IDENTIFIER);
        compiled = false;
    }
    //read :=
    if (isNextTokenCorrect() && next_token->getType() == TOKEN_KEYWORD &&
            ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_ASSIGNMENT)
    {
        readToken();
    }
    else {
        addError(SYN_ERROR_EXPECTED_ASSIGNMENT);
        compiled = false;
    }
    //read expr
    if (!compile_Expression()){
        SkipTokens({KEYWORD_TO, KEYWORD_DOWNTO}, false, false);
        compiled = false;
    }
    //read direction
    if (isNextTokenCorrect() && next_token->getType() == TOKEN_KEYWORD &&
            (
            ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_TO ||
            ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_DOWNTO))
    {
        readToken();
    }
    else {
        addError(SYN_ERROR_EXPECTED_DIRECTION);
        compiled = false;
    }
    //read expr
    if (!compile_Expression()){
        SkipTokens({KEYWORD_DO}, false, false);
        compiled = false;
    }

    //read do
    if (isNextTokenCorrect() && next_token->getType() == TOKEN_KEYWORD &&
            ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_DO)
    {
        readToken();
    }
    else {
        addError(SYN_ERROR_EXPECTED_DO);
        compiled = false;
    }

    compiled = compile_Operator() && compiled;
    return compiled;
}

bool Compiler::compile_RepeatOperator()
{
    bool compiled = true;
    readToken();
    if (!compile_Operator())
    {
        SkipTokens({KEYWORD_UNTIL}, false, false);
        compiled = false;
    }

    if (isNextTokenCorrect() && next_token->getType() == TOKEN_KEYWORD &&
            ((Token<PascalKeyword>*)next_token)->getData() == KEYWORD_UNTIL)
    {
        readToken();
    }
    else {
        addError(SYN_ERROR_EXPECTED_UNTIL);
        compiled = false;
    }

    compiled = compile_Expression() && compiled;
    return compiled;
}
