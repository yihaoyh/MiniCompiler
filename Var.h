#pragma once
#include"Lexer.h"
#include<string>
/*
	��������
*/
class Var
{
public :
	Tag tag; // �������ͱ�ǩ
	std::string name; // ��������
	std::string value_string; // ֵ���ַ�����ʽ
	
	Var();
	Var(Tag, std::string);
};

