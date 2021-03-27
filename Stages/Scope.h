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
    //������� ��� ������ ��� �������������
    AbstractType* createType(TypeType type);
    //��������� ���� ���
    void SetTypeName(Identifier id, AbstractType* type);

    //����� ��� �� ��������������. return null if no type
    AbstractType* getTypeById(Identifier id) const;
    //�������� ���������� ������������� ����
    Data* addVariable(Identifier id, AbstractType* type, bool is_const);
    //����� ������ �� ��������������
    Data* getDataById(Identifier id) const;
    std::vector<Identifier> getVariables() const;
    int getHeight() const;
    void print();

private:
    //������������ ������� ���������
    const Scope* parent;
    //��� ���� ������
    std::vector<AbstractType*> types;
    //����������� ���� ������ (��, ������� ��������� � ������� type)
    std::map<Identifier, AbstractType*> named_types;
    //����������
    std::map<Identifier, Data*> variables;
    int height; //������� �����
};

class RecordType : public AbstractType
{
public:
    RecordType(const Scope* parent) : scope(parent) {this->type = TYPE_RECORD;}
    const Scope* getScope() const {return &this->scope;}
protected:
    Scope scope;
};
