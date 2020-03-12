#pragma once
#include<string>
#include"Lexer.h"
#include"Type.h"
enum class VarType
{
	VAR, // ����
	TEMP, // ��ʱ����
	//LITERAL // ������
};

/*
	������
*/
class Var
{
public :
	static Var create_number(std::string value);
	static Var create_id(std::string name, Type type);
	Var();
	Var(Tag, std::string, std::string, Type type);
	Var& operator =(const Var& ref);

	Type type; // ��������
	Tag tag; // ������ǩ
	std::string name; // ��������
	std::string value_string; // ֵ���ַ�����ʽ
	static Var default_;
};

