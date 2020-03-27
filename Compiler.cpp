// Compiler.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <map>

#include "Common.h"
#include "Lexer.h"
#include "Parser.h"
#include "Utils.h"
extern std::map<std::string, Tag> keyword_table;

int main() {
  init_keyword_table();
  Parser parser;
  parser.begin_parse("array_test.txt");
  parser.post_parse();
  parser.print_instructions();
}
