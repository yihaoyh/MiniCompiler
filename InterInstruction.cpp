#include "InterInstruction.h"
#include<sstream>
#include"Utils.h"
InterInstruction::InterInstruction(Address result, Operator op, Address arg1, Address arg2) :result{ result }, op{ op }, arg1{ arg1 }, arg2{ arg2 }
{
}

std::string InterInstruction::to_string()
{
    std::stringstream ss;
    if (result.type == NAME)
    {
        ss << result.value << " = ";
    }
    else
    {
        error("result must be lvalue");
        return "";
    }
    if (arg1.type != EMPTY)
    {
        ss << arg1.value;
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
        break;
    case Operator::OP_SET_PARAM:
        ss << " set_param";
        break;
    case Operator::OP_RETURN:
        ss << " return ";
        break;
    default:
        ss << " unknown op";
        break;
    }
    if (arg2.type != EMPTY)
    {
        ss << arg2.value;
    }
    return ss.str();
}

Address var_to_address(const Var& var)
{
    if (var.tag == IDENTIFIER)
    {
        return Address{ NAME, var.name };
    }
    else if (var.tag == LT_NUMBER)
    {
        return Address{ LITERAL_NUMBER, var.value_string };
    }
    return Address{ UNKNOWN, "" };
}
