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
    static InterInstruction gen_if_jump(Operator op, Address arg1, Address arg2);
    static InterInstruction gen_if_false_jump(Operator op, Address arg1, Address arg2);
    static InterInstruction gen_jump();
    InterInstruction(const Address result, const Operator op, const Address arg1, const Address arg2);
    InterInstruction(const Inst_Type type, const Address result, const Operator op, const Address arg1, const Address arg2);
    static std::string type_to_string(const Inst_Type& type);
    std::string to_string();

    Address result;
    Operator op;
    Address arg1;
    Address arg2;
    Call call;
    unsigned int index = 0;  // ָ�����
    std::string label; // ָ����
    std::vector<unsigned int> next_list;
    Inst_Type type;
    bool removed = false;
    virtual ~InterInstruction() {};
};

Address var_to_address(const Var& var);

