#pragma once
#include"Lexer.h"
void init_keyword_table();
Tag get_keyword_tag(std::string);

enum class Operator
{
	OP_INVALID,		// invalid
	OP_ASSIGN,		// =
	OP_ADD,			// +
	OP_SUB,			// -
	OP_MUL,			// *
	OP_DIV,			// /
	OP_CALL,		// call ָ��
	OP_SET_PARAM,	// ���ò���
	OP_RETURN,		// ����
	OP_NEW_LABEL,	// ����һ���µı��
	OP_IF_JUMP,
	OP_IF_FALSE_JUMP, 
	OP_JUMP,
	OP_LESS,		// <
	OP_LESS_EQUAL,  // <=
	OP_EQUAL,		// ==
	OP_NOT_EQUAL,   // !=
	OP_GREATER,		// >
	OP_GREATER_EQUAL// >=
};

void print_error(const char* message);

typedef unsigned int instr_number;