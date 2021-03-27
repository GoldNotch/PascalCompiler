#include "CodeGenerator.h"
#include <cstdio>

CodeGenerator::CodeGenerator(const char* path)
{
    this->path = path;
    code_section << "MOV EBP, ESP\n";
}

CodeGenerator::~CodeGenerator()
{
    FILE* file = fopen(path, "w");
    fprintf(file, "%%include \"io.inc\"\n");
    fprintf(file, "section .data\n%s\n", data_section.str().c_str());
    fprintf(file, "section .bss\n%s\n", bss_section.str().c_str());
    code_section << "xor eax, eax\nret\n";
    fprintf(file, "\nsection .text\n\tglobal CMAIN\nCMAIN:\n%s\n", code_section.str().c_str());
    fclose(file);
}

void CodeGenerator::generateScope(const Scope* scope)
{
    std::vector<Identifier> variables = scope->getVariables();
    for (auto it = variables.begin(); it != variables.end(); it++)
    {
        const char* type = nullptr;
        Data* var = scope->getDataById(*it);
        if (var->isConst())
        {
            switch (var->getType()->getType())
            {
                case TYPE_SCALAR_4B: type = "DD"; break;
                case TYPE_SCALAR_1B: type = "DB"; break;
                case TYPE_REAL: type = "DD"; break;
                default: type = "DD"; break;
            }
            data_section << *it << "_" << scope->getHeight() << " " << type << " " << var->getValue() << std::endl;
        }
        else
        {
            switch (var->getType()->getType())
            {
                case TYPE_SCALAR_4B: type = "RESD"; break;
                case TYPE_SCALAR_1B: type = "RESB"; break;
                case TYPE_REAL: type = "RESD"; break;
                default: type = "RESD"; break;
            }
            bss_section << *it << "_" << scope->getHeight() << " " << type << " 1\n";
        }
    }
}

void CodeGenerator::pushVariable(const std::string& variable, bool negate)
{
    code_section << "MOV EAX, [" << variable << "]\n";
    if (negate)
        code_section << "NEG EAX\n";
    code_section << "PUSH EAX\n";
}

void CodeGenerator::pushValue(long long value)
{
    code_section << "push " << value << std::endl;
}

void CodeGenerator::ExecuteBinaryOperation(PascalKeyword operation)
{
    switch(operation)
    {
    case KEYWORD_PLUS:
        {
            code_section << "POP EDX\nPOP EAX\n";
            code_section << "ADD EAX, EDX\n";
        } break;
    case KEYWORD_MINUS:
        {
            code_section << "POP EDX\nPOP EAX\n";
            code_section << "SUB EAX, EDX\n";
        } break;
    case KEYWORD_SLASH:
        {
            code_section << "POP EBX\nPOP EAX\nCDQ\n";
            code_section << "IDIV EBX\n";
        } break;
    case KEYWORD_ASTERISK:
        {
            code_section << "POP EDX\nPOP EAX\n";
            code_section << "MUL EDX\n";
        } break;
    case KEYWORD_DIV:
        {
            code_section << "POP EBX\nPOP EAX\nCDQ\n";
            code_section << "IDIV EDX\n";
        } break;
    case KEYWORD_MOD:
        {
            code_section << "POP EBX\nPOP EAX\nCDQ\n";
            code_section << "IDIV EBX\nMOV EAX, EDX\n";
        } break;
    case KEYWORD_AND:
        {
            code_section << "POP EDX\nPOP EAX\n";
            code_section << "AND EAX, EDX\nAND EAX, 1\n";
        } break;
    case KEYWORD_OR:
        {
            code_section << "POP EDX\nPOP EAX\n";
            code_section << "OR EAX, EDX\nAND EAX, 1\n";
        } break;
    case KEYWORD_EQUAL:
        {
            code_section << "POP EDX\nPOP EAX\n";
            code_section << "CMP EAX, EDX\n"
                            "LAHF\n"
                            "SHR EAX, 14\n"
                            "AND EAX, 1\n";
        } break;
    case KEYWORD_NON_EQUAL:
        {
            code_section << "POP EDX\nPOP EAX\n";
            code_section << "CMP EAX, EDX\n"
                            "LAHF\n"
                            "SHR EAX, 14\n"
                            "NOT EAX\n"
                            "AND EAX, 1\n";
        } break;
    case KEYWORD_LESS_THAN:
        {
            code_section << "POP EDX\nPOP EAX\n";
            code_section << "CMP EAX, EDX\n"
                            "LAHF\n"
                            "SHR EAX, 14\n"
                            "AND EAX, 3\n"
                            "MOV EDX, EAX\n"
                            "SHR EAX, 1\n"
                            "AND EDX, 1\n"
                            "NOT EDX\n"
                            "AND EAX, EDX\n";
        } break;
    case KEYWORD_GREATER_THAN:
        {
            code_section << "POP EDX\nPOP EAX\n";
            code_section << "CMP EAX, EDX\n"
                            "LAHF\n"
                            "SHR EAX, 14\n"
                            "NOT EAX\n"
                            "AND EAX, 3\n"
                            "MOV EDX, EAX\n"
                            "SHR EAX, 1\n"
                            "AND EDX, 1\n"
                            "AND EAX, EDX\n";

        } break;
    case KEYWORD_LESS_THAN_OR_EQUAL:
        {
            code_section << "POP EDX\nPOP EAX\n";
            code_section << "CMP EAX, EDX\n"
                            "LAHF\n"
                            "SHR EAX, 14\n"
                            "AND EAX, 3\n"
                            "MOV EDX, EAX\n"
                            "SHR EAX, 1\n"
                            "AND EDX, 1\n"
                            "OR EAX, EDX\n";
        } break;
    case KEYWORD_GREATER_THAN_OR_EQUAL:
        {
            code_section << "POP EDX\nPOP EAX\n";
            code_section << "CMP EAX, EDX\n"
                            "LAHF\n"
                            "SHR EAX, 14\n"
                            "AND EAX, 3\n"
                            "MOV EDX, EAX\n"
                            "NOT EAX\n"
                            "SHR EAX, 1\n"
                            "AND EAX, 1\n"
                            "AND EDX, 1\n"
                            "OR EAX, EDX\n";
        } break;
    default: {} break;
    }
    code_section << "PUSH EAX\n";
}

void CodeGenerator::SaveToVariable(const std::string& variable)
{
    code_section << "POP EAX\n" << "MOV [" << variable << "], EAX\n";
}

void CodeGenerator::InputIntVariables(const std::vector<Data*>& variables)
{
    for(auto it = variables.begin(); it != variables.end(); it++){
        TypeType t = (*it)->getType()->getType();
        switch(t)
        {
            case TYPE_SCALAR_1B: code_section << "GET_DEC 1, " << (*it)->getName() << "\n"; break;
            case TYPE_SCALAR_4B: code_section << "GET_DEC 4, " << (*it)->getName() << "\n"; break;
            default: code_section << "GET_DEC 4, " << (*it)->getName() << "\n"; break;
        }
    }
}

void CodeGenerator::OutputIntVariables(const std::vector<Data*>& variables)
{
    for(auto it = variables.begin(); it != variables.end(); it++)
    {
        TypeType t = (*it)->getType()->getType();
        switch(t)
        {
            case TYPE_SCALAR_1B: code_section << "PRINT_DEC 1, " << (*it)->getName() << "\nPRINT_CHAR \' \'\n"; break;
            case TYPE_SCALAR_4B: code_section << "PRINT_DEC 4, " << (*it)->getName() << "\nPRINT_CHAR \' \'\n"; break;
            default: code_section << "PRINT_DEC 4, " << (*it)->getName() << "\nPRINT_CHAR \' \'\n"; break;
        }
    }
    code_section << "NEWLINE\n";
}

void CodeGenerator::ReserveLabel(std::string& label)
{
    label += "_" + std::to_string(last_label_id);
    last_label_id++;
}

int CodeGenerator::createLabel()
{
    code_section << "lbl_" << last_label_id << ":\n";
    return last_label_id++;
}

void CodeGenerator::createLabel(const std::string& label)
{
    code_section << label << ":\n";
}

void CodeGenerator::Jump(const std::string& label)
{
    code_section << "JMP " << label << "\n";
}

void CodeGenerator::JumpWithCondition(int then_label_id, int else_label_id)
{
    code_section << "POP EAX\n"
                    "CMP EAX, 1\n";
    if (then_label_id > 0)
        code_section << "JE lbl_" << then_label_id << "\n";
    if (else_label_id > 0)
        code_section << "JNE lbl_" << else_label_id << "\n";
}
