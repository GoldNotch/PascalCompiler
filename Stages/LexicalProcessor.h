#pragma once
#include <cstddef>
#include "../Types/Error.h"
#include "../Types/Token.h"
#include "../FileReader.h"
#include <map>

#include <string>



class LexicalProcessor
{
public:
    LexicalProcessor();
    ~LexicalProcessor();
    void bindReader(FileReader* reader);            //привязать читатель к процессору
    comp_error_t getNextToken(AbstactToken** token);//получить следующий токен
    void SaveTokenPosition(const std::string& reference);   //сохранить позицию токена + дать ей имя
    void RollbackToToken(const std::string& reference);     //откатить лексер к предыдущему токену
    void PopSavedTokenPosition(const std::string& reference);//убрать ссылку на позицию токена
private:
    FileReader* reader;
    std::map<std::string, TextPosition> positions;
};

