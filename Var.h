#pragma once
#include"Lexer.h"
#include<string>
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
	VarType type; // ��������
	Tag tag; // ������ǩ
	std::string name; // ��������
	std::string value_string; // ֵ���ַ�����ʽ
	static Var default_;
	Var();
	Var(Tag, std::string, VarType type);
	Var& operator =(const Var& ref);
};

