#pragma once
#include<map>
#include<string>
#include<vector>
#include"Var.h"
//#include"Function.h"
#include"InterInstruction.h"

//class Function;

class SymTable
{
public:
	void put_variable(Var);
	Var get_variable(std::string);
	//void add_instruction(InterInstruction);
	unsigned int get_next_instruction();
	Var gen_temp_var(Type type); // ������ʱ����
private:
	std::map<std::string, Var> var_table_;  // ������
	std::map<std::string, Var> temp_var_table; // ��ʱ������
	std::vector<InterInstruction> inst_vector;
};