#include "Compiler.h"
#include "Stages/LexicalProcessor.h"
#include "Types/PascalKeywords.h"
#include <memory>

std::vector<comp_error_t> Compile(const char* text, void* const program)
{
    std::vector<comp_error_t> errors;
    LexicalProcessor lexer;
    lexer.bindText(text);
    AbstactToken* token;
    comp_error_t err;
    do{
        err = lexer.getNextToken(&token);
        if (token && !err.code)
        {
            printf("%s\n", token->toString().c_str());
            delete token;
        }
        else if (err.code)
            errors.push_back(err);
    } while(err.code != LEX_ERROR_END_OF_FILE);
    return errors;
}
