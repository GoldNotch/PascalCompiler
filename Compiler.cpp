#include "Compiler.h"
#include "Stages/LexicalProcessor.h"
#include "Types/PascalKeywords.h"

std::vector<comp_error_t> Compile(const char* text, void* const program)
{
    std::vector<comp_error_t> errors;
    LexicalProcessor lexer;
    lexer.bindText(text);
    AbstactToken* token;
    comp_error_t err;
    do{
        err = lexer.getNextToken(&token);
        if (token)
        {

            switch(token->getType())
            {
            case TOKEN_KEYWORD:
                {
                    PascalKeyword key = ((Token<PascalKeyword>*)token)->getData();
                    printf("keyword - %i\n", key);
                } break;
            case TOKEN_IDENTIFIER:
                {
                    std::string ident = ((Token<std::string>*)token)->getData();
                    printf("identifier - %s\n", ident.c_str());
                } break;
            case TOKEN_CONST:
                {
                    AbstactConstant* constant = ((Token<AbstactConstant*>*)token)->getData();
                    switch(constant->getType())
                    {
                        case CONST_INT: printf("int constant -  %i\n", ((Constant<int>*)constant)->getValue()); break;
                        case CONST_CHAR: printf("char constant - %c\n", ((Constant<char>*)constant)->getValue()); break;
                        case CONST_REAL: printf("real constant - %f\n", ((Constant<float>*)constant)->getValue()); break;
                        case CONST_STRING:{
                            std::string t = ((Constant<std::string>*)constant)->getValue();
                            printf("string constant - %s\n", t.c_str());
                        }  break;
                    }
                } break;
                default: break;
            }
            delete token;
        }
    } while(err.code != LEX_ERROR_END_OF_FILE);
    return errors;
}
