#pragma once
#include"Lexer.h"
#include<string>
enum class VarType
{
	VAR, // 变量
	TEMP, // 临时变量
	//LITERAL // 字面量
};

/*
	变量类
*/
class Var
{
public :
	VarType type; // 变量类型
	Tag tag; // 变量标签
	std::string name; // 变量名称
	std::string value_string; // 值的字符串形式
	static Var default_;
	Var();
	Var(Tag, std::string, VarType type);
	Var& operator =(const Var& ref);
};

