#pragma once
#include<string>
#include"Var.h"
#include"Common.h"
#include"Call.h"
/*
 * ����Ԫʽ��ʾ�м�ָ��
 */
class InterInstruction
{
public:
    InterInstruction(Var result, Operator op, Var arg1, Var arg2);
    std::string to_string();
    //InterInstruction(Call call);
	Var result;
    Operator op;
    Var arg1;
    Var arg2;
    Call call;
};

