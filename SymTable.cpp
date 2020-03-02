#include "SymTable.h"
#include<sstream>
void SymTable::put_variable(Var var)
{
	var_table_[var.name] = var;
}

void SymTable::put_function(Function fun)
{
	function_table_[fun.name] = fun;
}

Var SymTable::get_variable(std::string name)
{	
	if (var_table_.find(name) != var_table_.end()) {
		return var_table_[name];
	}
	return Var();
}

Function SymTable::get_function(std::string name)
{
	if (function_table_.find(name) != function_table_.end())
	{
		return function_table_[name];
	}
	return Function();
}

void SymTable::add_instruction(InterInstruction inst)
{
	inst_vector.push_back(inst);
}

Var SymTable::gen_temp_var()
{
	std::stringstream sstream;
	sstream << "temp_" << temp_var_table.size();
	std::string name = std::string(sstream.str());
	Var var = Var(IDENTIFIER, name, VarType::TEMP);
	temp_var_table[name] = var;
	return var;
}

void SymTable::set_current_function(Function* function)
{
	if (function != nullptr)
	{
		current_function_ = function;
	}
}

Function* SymTable::get_current_function()
{
	return current_function_;
}

std::vector<Function> SymTable::get_functions()
{
	std::vector<Function> functions;
	for (auto iter = function_table_.begin(); iter != function_table_.end(); iter++)
	{
		functions.push_back(iter->second);
	}
	return functions;
}
