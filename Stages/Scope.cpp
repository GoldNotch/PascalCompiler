#include "Scope.h"

Scope::Scope(const Scope* parent)
{
    this->parent = parent;
}

Scope::~Scope()
{
    for(auto it = variables.begin(); it != variables.end(); it++)
    {
        delete it->second;
    }
    variables.clear();
    named_types.clear();
    for(size_t i = 0; i < types.size(); i++)
    {
        delete types[i];
        types[i] = nullptr;
    }
    types.clear();
}

AbstractType* Scope::createType(TypeType type)
{
    AbstractType* new_type = nullptr;
    switch(type)
    {
        case TYPE_SCALAR: new_type = new ScalarType(); break;
        case TYPE_REAL: new_type = new RealType(); break;
        case TYPE_RECORD: new_type = new RecordType(this); break;
        default: break;
    }
    types.push_back(new_type);
    return new_type;
}

void Scope::SetTypeName(Identifier id, AbstractType* type)
{
    this->named_types[id] = type;
}


AbstractType* Scope::getTypeById(Identifier id) const
{
    const Scope* s = this;
    AbstractType* type = nullptr;
    do
    {
        auto it = s->named_types.find(id);
        type = it == s->named_types.end() ? nullptr : it->second;
        s = s->parent;
    }
    while (type == nullptr && s != nullptr);
    return type;
}

void Scope::addVariable(Identifier id, AbstractType* type, bool is_const)
{
    Data* new_data = new Data(type, is_const);
    this->variables[id] = new_data;
}

Data* Scope::getDataById(Identifier id) const
{
    const Scope* s = this;
    Data* data = nullptr;
    do
    {
        auto it = s->variables.find(id);
        data = it == s->variables.end() ? nullptr : it->second;
        s = s->parent;
    }
    while (data == nullptr && s != nullptr);
    return data;
}


void  Scope::print()
{
    printf("types\n");
    for(auto it = this->named_types.begin(); it != this->named_types.end(); it++)
        printf("%s\n", it->first.c_str());
    printf("variables\n");
    for(auto it = this->variables.begin(); it != this->variables.end(); it++)
        printf("%s\n", it->first.c_str());
}
