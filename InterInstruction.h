#pragma once
#include<string>
#include<vector>
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

enum class Inst_Type
{
    NORMAL,
    IF_JUMP,
    IF_FALSE_JUMP,
    JUMP
};

/*
 * ����Ԫʽ��ʾ�м�ָ��
 */
class InterInstruction
{
public:
    InterInstruction(const Address result, Operator op, Address arg1, Address arg2);
    static InterInstruction gen_if_jump(Operator op, Address arg1, Address arg2);
    static InterInstruction gen_if_false_jump(Operator op, Address arg1, Address arg2);
    static InterInstruction gen_jump();
    std::string to_string();
    //InterInstruction(Call call);
    Address result;
    Operator op;
    Address arg1;
    Address arg2;
    Call call;
    unsigned int index = 0;  // ָ�����
    std::vector<unsigned int> next_list;
    //unsigned int next_index = 0; // ��һ��ָ������
    Inst_Type type;
    virtual ~InterInstruction() {};
};

Address var_to_address(const Var& var);

