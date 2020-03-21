#pragma once
#include<vector>
#include<string>
#include"Var.h"
#include"SymTable.h"
#include"InterInstruction.h"
//class SymTable;

class Function
{
public:
	Function(const std::string& fun_name, bool is_declaration, const Type& type, const std::vector<Var>& params);
	Function();
    unsigned int add_instruction(InterInstruction& inst);
	InterInstruction& get_instruction(unsigned int index);
	std::vector<InterInstruction>& get_instructions();
	const std::vector<Var> get_params();
	unsigned int get_next_instruction();
	void put_variable(const Var& var);
	Var get_variable(const std::string& name);
	Var gen_temp_var(const Type type);
	std::string gen_label();

	bool is_declaration;  // true表示声明，false表示定义
	std::string name; // 函数名称
	SymTable* get_sym_table();
	Type return_type = Type::UNKNOWN; 
private:
	SymTable sym_table;
	std::vector<Var> params_;
	std::vector<InterInstruction> inst_vector_;
	unsigned int label_index_ = 0;
};

