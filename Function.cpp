#include "Function.h"

Function::Function(std::string fun_name, bool declaration) :name{ fun_name }, is_declaration{declaration}
{
	//sym_table_ = SymTable();
}

Function::Function() : name{ "" }, is_declaration{ "false" }
{

}

void Function::add_instruction(InterInstruction inst)
{
	inst_vector_.push_back(inst);
}

std::vector<InterInstruction>& Function::get_instructions()
{
	return inst_vector_;
}

//void Function::put_variable(Var var)
//{
//	sym_table_.put_variable(var);
//}
