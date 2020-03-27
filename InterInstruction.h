#pragma once
#include <string>
#include <vector>

#include "Common.h"
#include "Var.h"

/*
 * 表示三地址码的地址
 */
// TODO 定义和Type重复了
constexpr auto EMPTY = -1;
constexpr auto NAME = 0;
constexpr auto TEMP_VAR = 1;
constexpr auto LITERAL_NUMBER = 2;
constexpr auto LITERAL_CHAR = 3;
constexpr auto LITERAL_STRING = 4;
constexpr auto POINTER = 5;

struct Address_ {
   // TODO 缺少数据类型
  int type = -1;
  /*
   值的字符串形式，type为NAME时为名字，为字面量时需要从字符串进行转换。
  */
  std::string value;

  static Address_ label_addr(instr_number number);
};
typedef Address_ Address;

enum class Inst_Type { NORMAL, IF_JUMP, IF_FALSE_JUMP, JUMP };

/*
 * 用四元式表示中间指令
 */
class InterInstruction {
 public:
  static InterInstruction gen_if_jump(Operator op, Address arg1, Address arg2);
  static InterInstruction gen_if_false_jump(Operator op, Address arg1,
                                            Address arg2);
  static InterInstruction gen_jump();
  static InterInstruction gen_jump(instr_number number);
  InterInstruction(const Address result, const Operator op, const Address arg1,
                   const Address arg2);
  InterInstruction(const Inst_Type type, const Address result,
                   const Operator op, const Address arg1, const Address arg2);
  static std::string type_to_string(const Inst_Type& type);
  std::string to_string();

  Address result;
  Operator op;
  Address arg1;
  Address arg2;
  // Call call;
  unsigned int index = 0;  // 指令序号
  std::string label;       // 指令标号
  std::vector<unsigned int> next_list;
  Inst_Type type;
  bool removed = false;
  virtual ~InterInstruction(){}
};

Address var_to_address(const Var& var);
