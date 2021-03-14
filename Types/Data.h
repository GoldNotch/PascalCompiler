#pragma once
#include "Type.h"

class Data
{
public:
    Data(AbstractType* type, bool is_const)
    {
        this->type = type;
        this->is_const = is_const;
    }
    ~Data() {
        type = nullptr;
    }
    bool isConst() const {return is_const;}
    AbstractType* getType() const {return this->type;}
private:
    bool is_const;
    AbstractType* type;
};

