#pragma once
#include <vector>

enum GrammaticType
{
    GRAMMATIC_UNKNOWN,
    GRAMMATIC_EXPRESSION,
    GRAMMATIC_SIMPLE_EXPRESSION,
    GRAMMATIC_COMPARE_OPERATION,
    GRAMMATICS_TOTAL
};

class AbstactGrammatic
{
public:
    AbstactGrammatic(AbstactGrammatic* parent, GrammaticType type) : type(type), parent(parent), children(nullptr);
    {}
    virtual ~AbstactGrammatic()
    {
        parent = null;
        for(size_t i = 0; i < children.size(); i++)
            delete children[i];
    }
    GrammaticType getType(){return type;}
private:
    GrammaticType type;
    AbstactGrammatic* parent;
    std::vector<AbstactGrammatic*> children;
};
