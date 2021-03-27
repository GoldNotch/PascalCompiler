#pragma once
#include "../Types/Data.h"
#include "Scope.h"
#include <sstream>
#include "../Types/PascalKeywords.h"

class CodeGenerator
{
public:
    CodeGenerator(const char* path);
    ~CodeGenerator();
    //создание переменных
    void generateScope(const Scope* scope);
    //вычисление выражения через постфиксную запись
    void pushVariable(const std::string& variable, bool negate);
    void pushValue(long long value);
    void ExecuteBinaryOperation(PascalKeyword operation);
    void SaveToVariable(const std::string& variable);
    //ввод/вывод
    void InputIntVariables(const std::vector<Data*>& variables);
    void OutputIntVariables(const std::vector<Data*>& variables);
    //переходы в программе
    void ReserveLabel(std::string& label);
    int createLabel();
    void createLabel(const std::string& label);
    void Jump(const std::string& label);
    void JumpWithCondition(int then_label_id, int else_label_id);
private:
    const char* path;
    std::stringstream data_section;
    std::stringstream bss_section;
    std::stringstream code_section;
    int last_label_id = 1;
};
