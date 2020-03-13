#include "Function.h"

Function::Function(const std::string& fun_name, bool is_declaration, const Type& type, const std::vector<Var>& params) :name{ fun_name }, is_declaration{ is_declaration }, return_type{ type }, params_{ params }
{
    for (auto iter = params.begin(); iter != params.end(); ++iter)
    {
        sym_table.put_variable(*iter);
    }
}

Function::Function() : name{ "" }, is_declaration{ "false" }
{
    return_type = Type::UNKNOWN;
}

void Function::add_instruction(InterInstruction inst)
{
    inst_vector_.push_back(inst);
}

const std::vector<InterInstruction>& Function::get_instructions()
{
    return inst_vector_;
}

const std::vector<Var> Function::get_params()
{
    return params_;
}

void Function::put_variable(const Var& var)
{
    sym_table.put_variable(var);
}

Var Function::get_variable(const std::string& name)
{
    return  sym_table.get_variable(name);
}

Var Function::gen_temp_var(const Type type)
{
    Var var  = sym_table.gen_temp_var(type);
    put_variable(var);
    return var;
}

SymTable* Function::get_sym_table()
{
    return &sym_table;
}
