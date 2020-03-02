#pragma once
#include<vector>
#include<string>
#include"Var.h"
//#include"SymTable.h"
#include"InterInstruction.h"
class Function
{
public:
	bool is_declaration;  // true��ʾ������false��ʾ����
	std::string name; // ��������

	Function(std::string, bool);
	Function();
	//void put_variable(Var var);
	void add_instruction(InterInstruction inst);
	std::vector<InterInstruction>& get_instructions();
private:
	std::vector<Var> params_;
	std::vector<InterInstruction> inst_vector_;
	//SymTable sym_table_;
};

