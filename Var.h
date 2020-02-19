#pragma once
#include"Lexer.h"
#include<string>
/*
	变量类型
*/
class Var
{
public :
	Tag tag; // 变量类型标签
	std::string name; // 变量名称
	std::string value_string; // 值的字符串形式
	
	Var();
	Var(Tag, std::string);
};

