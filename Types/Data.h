#pragma once
#include "Type.h"

class Data
{
public:
    Data(std::string name, AbstractType* type, bool is_const)
    {
        this->type = type;
        this->is_const = is_const;
        default_value = 0ll;
        this->name = name;
    }
    ~Data() {
        type = nullptr;
    }
    bool isConst() const {return is_const;}
    AbstractType* getType() const {return this->type;}
    void setValue(long long value){this->default_value = value;}
    long long getValue() const {return default_value;}
    const std::string& getName() const {return name;}
private:
    bool is_const;
    AbstractType* type;
    long long default_value;
    std::string name;
};

