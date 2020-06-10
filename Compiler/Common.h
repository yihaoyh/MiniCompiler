#pragma once
#include <string>

#include "Lexer.h"
void init_keyword_table();
Tag get_keyword_tag(std::string);

enum class Operator {
  OP_INVALID,     // invalid
  OP_ASSIGN,      // =
  OP_ADD,         // +
  OP_SUB,         // -
  OP_MUL,         // *
  OP_DIV,         // /
  OP_CALL,        // call 指令
  OP_PUSH_PARAM,  // 参数压栈
  OP_POP_PARAM,   // 参数出栈
  OP_RETURN,      // 返回
  OP_NEW_LABEL,   // 创建一个新的标号
  OP_IF_JUMP,
  OP_IF_FALSE_JUMP,
  OP_JUMP,
  OP_LESS,           // <
  OP_LESS_EQUAL,     // <=
  OP_EQUAL,          // ==
  OP_NOT_EQUAL,      // !=
  OP_GREATER,        // >
  OP_GREATER_EQUAL,  // >=
  OP_ARRAY          // 数组偏移
};

void print_error(const char* message);

typedef unsigned int instr_number;