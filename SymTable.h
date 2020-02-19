#pragma once
#include<map>
#include<string>
#include"Var.h"
class SymTable
{
public:
	void put_sym(Var);
private:
	std::map<std::string, Var> var_table_;
};

