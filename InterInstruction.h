#pragma once
#include<string>
#include"Var.h"
#include"Common.h"
#include"Call.h"

/*
 * ��ʾ����ַ��ĵ�ַ
 */

constexpr auto EMPTY = -1;
constexpr auto NAME = 0;
constexpr auto TEMP_VAR = 1;
constexpr auto LITERAL_NUMBER = 2;
constexpr auto LITERAL_CHAR = 3;
constexpr auto LITERAL_STRING = 4;

struct Address_
{
    int type;
    std::string value;   // ֵ���ַ�����ʽ��typeΪNAMEʱΪ���֣�Ϊ������ʱ��Ҫ���ַ�������ת����
};
typedef Address_ Address;

/*
 * ����Ԫʽ��ʾ�м�ָ��
 */
class InterInstruction
{
public:
    InterInstruction(Address result, Operator op, Address arg1, Address arg2);
    std::string to_string();
    //InterInstruction(Call call);
    Address result;
    Operator op;
    Address arg1;
    Address arg2;
    Call call;
};

Address var_to_address(const Var& var);

