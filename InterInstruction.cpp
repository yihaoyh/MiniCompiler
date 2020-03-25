#include "InterInstruction.h"

#include <sstream>

#include "Utils.h"
InterInstruction::InterInstruction(const Address result, const Operator op,
                                   const Address arg1, const Address arg2)
    : result{result}, op{op}, arg1{arg1}, arg2{arg2} {
  type = Inst_Type::NORMAL;
}

InterInstruction::InterInstruction(const Inst_Type type, const Address result,
                                   const Operator op, const Address arg1,
                                   const Address arg2)
    : type{type}, result{result}, op{op}, arg1{arg1}, arg2{arg2} {}

std::string InterInstruction::type_to_string(const Inst_Type& type) {
  switch (type) {
    case Inst_Type::NORMAL:
      return "normal";
    case Inst_Type::IF_JUMP:
      return "if";
    case Inst_Type::IF_FALSE_JUMP:
      return "if_false";
    case Inst_Type::JUMP:
      return "goto";
    default:
      return "normal";
  }
}

InterInstruction InterInstruction::gen_if_jump(Operator op, Address arg1,
                                               Address arg2) {
  return InterInstruction(Inst_Type::IF_JUMP, Address(), op, arg1, arg2);
}

InterInstruction InterInstruction::gen_if_false_jump(Operator op, Address arg1,
                                                     Address arg2) {
  return InterInstruction(Inst_Type::IF_FALSE_JUMP, Address(), op, arg1, arg2);
}

InterInstruction InterInstruction::gen_jump() {
  return InterInstruction(Inst_Type::JUMP, Address::label_addr(0), Operator::OP_JUMP,
                          Address(), Address());
}

InterInstruction InterInstruction::gen_jump(instr_number number) {
  return InterInstruction(Inst_Type::JUMP, Address::label_addr(number), Operator::OP_JUMP,
                          Address(), Address());
}

std::string InterInstruction::to_string() {
  std::stringstream ss;
  ss << InterInstruction::type_to_string(type) << " ";
  if (result.type == NAME) {
    ss << result.value << " = ";
  }
  if (arg1.type != EMPTY) {
    ss << arg1.value;
  }
  switch (op) {
    case Operator::OP_ADD:
      ss << " + ";
      break;
    case Operator::OP_SUB:
      ss << " - ";
      break;
    case Operator::OP_MUL:
      ss << " * ";
      break;
    case Operator::OP_DIV:
      ss << " / ";
      break;
    case Operator::OP_CALL:
      ss << " call ";
      break;
    case Operator::OP_ASSIGN:
      break;
    case Operator::OP_PUSH_PARAM:
      ss << " push_param";
      break;
    case Operator::OP_POP_PARAM:
      ss << " pop_param";
      break;
    case Operator::OP_RETURN:
      ss << " return ";
      break;
    default:
      ss << " unknown op";
      break;
  }
  if (arg2.type != EMPTY) {
    ss << arg2.value;
  }
  return ss.str();
}

Address var_to_address(const Var& var) {
  if (var.tag == Tag::IDENTIFIER) {
    return Address{NAME, var.name};
  } else if (var.tag == Tag::LT_NUMBER) {
    return Address{LITERAL_NUMBER, var.value_string};
  }
  return Address{EMPTY, ""};
}

Address_ Address_::label_addr(instr_number number) { 
    return Address{LITERAL_STRING, std::to_string(number)};
}
