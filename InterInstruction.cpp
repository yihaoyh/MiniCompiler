#include "InterInstruction.h"
#include<sstream>
InterInstruction::InterInstruction(Var result, Operator op, Var arg1, Var arg2) :result{ result }, op{ op }, arg1{arg1},arg2{arg2}
{
}

std::string InterInstruction::to_string()
{
	std::stringstream ss;
	if (result.tag != UNKNOWN)
	{
		ss << result.name << " = ";
	}
	if (arg1.tag != UNKNOWN)
	{
		if (arg1.tag == IDENTIFIER)
		{
			ss << arg1.name;
		}
		else if (arg1.tag == LT_NUMBER || arg1.tag == LT_CHAR || arg1.tag == LT_STRING)
		{
			ss << arg1.value_string;
		}
		//ss << arg1.name;
	}
	switch (op) 
	{
	case Operator::OP_ADD:
		ss << " + ";
		break;
	case Operator::OP_SUB:
		ss << " - ";
		break;
	case Operator::OP_MUL:
		ss << " * ";
		break;
	case Operator::OP_DIV:
		ss << " / ";
		break;
	case Operator::OP_CALL:
		ss << " call ";
		break;
	case Operator::OP_ASSIGN:
		ss << " = ";
		break;
	case Operator::OP_RETURN:
		ss << " return ";
		break;
	default:
		ss << " unknown op";
		break;
	}
	if (arg2.tag != UNKNOWN)
	{
		if (arg2.tag == IDENTIFIER)
		{
			ss << arg2.name;
		}
		else if (arg2.tag == LT_NUMBER || arg2.tag == LT_CHAR || arg2.tag == LT_STRING)
		{
			ss << arg2.value_string;
		}
	}
	return ss.str();
}

//InterInstruction::InterInstruction(Call call)
//{
//}
