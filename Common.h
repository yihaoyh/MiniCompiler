#pragma once
#include"Lexer.h"
void init_keyword_table();
Tag get_keyword_tag(std::string);

enum class Operator
{
	OP_INVALID, // invalid
	OP_ASSIGN, // =
	OP_ADD, // +
	OP_SUB, // -
	OP_MUL, // *
	OP_DIV, // /
	OP_CALL, // call 指令
	OP_SET_PARAM, // 设置参数
	OP_RETURN // 返回
};