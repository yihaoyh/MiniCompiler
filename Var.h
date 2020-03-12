#pragma once
#include<string>
#include"Lexer.h"
#include"Type.h"
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
	static Var create_number(std::string value);
	static Var create_id(std::string name, Type type);
	Var();
	Var(Tag, std::string, std::string, Type type);
	Var& operator =(const Var& ref);

	Type type; // 变量类型
	Tag tag; // 变量标签
	std::string name; // 变量名称
	std::string value_string; // 值的字符串形式
	static Var default_;
};

