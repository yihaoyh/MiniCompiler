#include "SymTable.h"
#include<sstream>
void SymTable::put_variable(Var var)
{
	var_table_[var.name] = var;
}

Var SymTable::get_variable(std::string name)
{	
	if (var_table_.find(name) != var_table_.end()) {
		return var_table_[name];
	}
	return Var::default_;
}

void SymTable::add_instruction(InterInstruction inst)
{
	inst_vector.push_back(inst);
}

Var SymTable::gen_temp_var(Type type)
{
	std::stringstream sstream;
	sstream << "temp_" << temp_var_table.size();
	std::string name = std::string(sstream.str());
	Var var = Var::create_id(name, type);
	temp_var_table[name] = var;
	return var;
}


