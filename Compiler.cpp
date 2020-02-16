// Compiler.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include<map>
#include"Utils.h"
#include"Lexer.h"
#include"Common.h"
extern std::map<std::string, Tag> keyword_table;

int main()
{
	init_keyword_table();
	Lexer lexer;
	lexer.scan("test.txt");
}

void init()
{

}

