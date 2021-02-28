#pragma once
#include <cstddef>
#include "../Types/Error.h"
#include "../Types/Token.h"
#include "../FileReader.h"

#include <string>

class LexicalProcessor
{
public:
    LexicalProcessor();
    ~LexicalProcessor();
    void bindReader(FileReader* reader);            //привязать читатель к процессору
    comp_error_t getNextToken(AbstactToken** token);//получить следующий токен
private:
    FileReader* reader;
};

