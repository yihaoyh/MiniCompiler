#pragma once
#include"Var.h"
class Param
{
public:
	Var var;
	bool isEscape;    // �Ƿ�����

	Param(Var var, bool isEscape) :var{ var },isEscape{ isEscape }
	{
	}

	Param& operator = (const Param& ref);

};

