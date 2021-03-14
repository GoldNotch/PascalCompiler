#pragma once
#include <map>
#include <string>
#include <vector>
#include "../Types/Data.h"
#include "../Types/Type.h"

typedef std::string Identifier;

class Scope
{
public:
    Scope(const Scope* parent_scope = nullptr);
    ~Scope();
    //создать тип данных как целочисленный
    AbstractType* createType(TypeType type);
    //присвоить типу имя
    void SetTypeName(Identifier id, AbstractType* type);

    //найти тип по идентификатору. return null if no type
    AbstractType* getTypeById(Identifier id) const;
    //добавить переменную определенного типа
    void addVariable(Identifier id, AbstractType* type, bool is_const);
    //найти данные по идентификатору
    Data* getDataById(Identifier id) const;

    void print();

private:
    //родительская область видимости
    const Scope* parent;
    //все типы данных
    std::vector<AbstractType*> types;
    //именованные типы данных (те, которые объявлены в разделе type)
    std::map<Identifier, AbstractType*> named_types;
    //переменные
    std::map<Identifier, Data*> variables;
};

class RecordType : public AbstractType
{
public:
    RecordType(const Scope* parent) : scope(parent) {this->type = TYPE_RECORD;}
    const Scope* getScope() const {return &this->scope;}
protected:
    Scope scope;
};
