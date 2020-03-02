#pragma once
#include<map>
#include<string>
#include<vector>
#include"Var.h"
#include"Function.h"
#include"InterInstruction.h"
class SymTable
{
public:
	void put_variable(Var);
	void put_function(Function);
	Var get_variable(std::string);
	Function get_function(std::string);
	void add_instruction(InterInstruction);
	Var gen_temp_var(); // ������ʱ����
	void set_current_function(Function*);
	Function* get_current_function();
	std::vector<Function> get_functions();
private:
	std::map<std::string, Var> var_table_;  // ������
	std::map<std::string, Var> temp_var_table; // ��ʱ������
	std::map<std::string, Function> function_table_; // ������
	std::vector<InterInstruction> inst_vector;
	Function* current_function_;
};