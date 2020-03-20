#pragma once
#include<string>
#include<vector>
#include"Var.h"
#include"Common.h"
#include"Call.h"

/*
 * 表示三地址码的地址
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
    std::string value;   // 值的字符串形式，type为NAME时为名字，为字面量时需要从字符串进行转换。
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
 * 用四元式表示中间指令
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
    unsigned int index = 0;  // 指令序号
    std::vector<unsigned int> next_list;
    //unsigned int next_index = 0; // 下一个指令的序号
    Inst_Type type;
    virtual ~InterInstruction() {};
};

Address var_to_address(const Var& var);

