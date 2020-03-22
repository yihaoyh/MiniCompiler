#include "CodeGen.h"

#include <assert.h>

#include <sstream>

#include "Common.h"
#include "Utils.h"

#define VAR_LENGTH 8
#define PARAM_OFFSET_BEGIN 16
#define LOCAL_OFFSET_BEGIN 0
const std::string RAX = "%rax";
const std::string RBX = "%rbx";
const std::string RCX = "%rcx";

std::string CodeGen::parse_functions(const std::vector<Function>& functions) {
  std::string code = "";
  for (auto iter = functions.cbegin(); iter != functions.cend(); ++iter) {
    function_ = &(*iter);
    curr_frame_ = Frame(nullptr, iter->name, iter->get_params());
    parse_params();
    code += parse_function(*function_);
  }
  return code;
}

std::string CodeGen::parse_function(const Function& fun) {
  std::vector<InterInstruction> inst_list = fun.get_const_instructions();
  std::string code = "";
  code += gen_header(curr_frame_.fun_name);
  code += gen_enter_proc();
  for (auto iter = inst_list.begin(); iter != inst_list.end(); ++iter) {
    if (iter->removed) {
      continue;
    }
    code += parse_instruction(*iter);
  }
  code += gen_exit_proc();
  return code;
}

std::string CodeGen::parse_instruction(const InterInstruction& inst) {
  Var result = get_var(inst.result);
  Var arg1 = get_var(inst.arg1);
  Var arg2 = get_var(inst.arg2);
  std::string code = "";
  if (!inst.label.empty()) {
    code += inst.label + ":\n";
  }
  if (inst.type == Inst_Type::IF_JUMP) {
    return code + gen_if_jump(inst.op, result, arg1, arg2);
  } else if (inst.type == Inst_Type::IF_FALSE_JUMP) {
    return code + gen_if_false_jump(inst.op, result, arg1, arg2);
  } else if (inst.type == Inst_Type::JUMP) {
    return code + gen_jump(result);
  }
  // normal
  switch (inst.op) {
    case Operator::OP_ADD:
    case Operator::OP_SUB:
      if (result.tag != Tag::IDENTIFIER) {
        error("result must be lvalue in add/sub");
        return "";
      }
      if (arg1.tag != Tag::IDENTIFIER && arg1.tag != Tag::LT_NUMBER) {
        error("arg1 must be number or id in add/sub");
      }
      if (arg2.tag != Tag::IDENTIFIER && arg2.tag != Tag::LT_NUMBER) {
        error("arg2 must be number or id in add/sub");
      }
      return code + gen_low_op(inst.op, result, arg1, arg2);
    case Operator::OP_ASSIGN:
      if (result.tag != Tag::IDENTIFIER) {
        error("result must be lvalue in add/sub");
        return "";
      }
      return code + gen_assign(result, arg1);
    case Operator::OP_SET_PARAM:
      return code + gen_set_param(arg1);
    case Operator::OP_CALL:
      code = code + gen_call(result, inst.arg1.value);
      return code;
    case Operator::OP_RETURN:
      return code + gen_return(arg1);
    case Operator::OP_IF_JUMP:
      return code + gen_if_jump(inst.op, result, arg1, arg2);
    default:
      return "";
  }
}

std::string CodeGen::gen_header(const std::string& fun_name) {
  if (fun_name.empty()) {
    return "";
  }
  std::stringstream sstream;
  if (fun_name == "main") {
    sstream << ".LC0:\n"
               "\t.string	\"%d\\n\"\n"
               "\t.text\n";
  } else {
    sstream << "\t.text\n";
  }
  sstream << "\t.globl  " << fun_name << "\n";
  sstream << fun_name << ":"
          << "\n";
  return sstream.str();
}

/*
 * 生成赋值指令，为减少工作量，只支持64位整形
 * lval 左值，用rbx寄存器
 * rval 右值，用rcx寄存器或者立即数
 * 关键指令形式类似 mov %rcx, %rbx 或者 mov $10, %rbx
 */
std::string CodeGen::gen_assign(const Var& lval, const Var& rval) {
  // 判断rval是变量还是字面量
  // 如果是字面量，生成字面量字符串形式如 str_rval = "$10"
  // 如果是变量，则判断该变量在栈帧信息中是否存在。
  // 如果存在，则生成一条指令，该指令根据变量的偏移地址，读入到ecx寄存器中，并使str_rval
  // = "%rcx" 如果不存在，则返回 生成指令 movq $str_rval, %rbx
  // 然后判断lval是否存在于栈帧信息中，如果存在，则生成指令将rbx写入到相应内存中。
  // 如果不存在，则生成指令 push
  // rbx，并调用Frame::insert_variable向栈帧信息添加一个变量
  std::string code = "";
  std::string str_rval = "";
  if (lval.tag != Tag::IDENTIFIER) {
    return "";
  }
  code += gen_access_arg(rval, RBX);
  if (curr_frame_.is_variable_exists(lval.name)) {
    int offset = curr_frame_.get_variable_offset(lval.name);
    if (!frame_offset_check(lval.name, offset)) {
      return "";
    }
    code += gen_save_variable(RBX, offset);
  } else {
    code += gen_create_variable(RBX);
    curr_frame_.add_local(lval.name, get_type_length(lval.type));
  }
  return code;
}

/*
 * 生成加/减法指令，为减少工作量，只支持64位整形
 * result 结果，必须为变量
 * lval 左值
 * rval 右值
 */
std::string CodeGen::gen_low_op(Operator op, const Var& result, const Var& lval,
                                const Var& rval) {
  // 判断result的tag是否为ID，若不是，则返回
  // 如果lval是整形字面量，则str_lval为"$number"
  // 否则根据栈帧信息，生成一条指令，该指令从内存中读取lval的值到rbx,
  // str_lval设置为"%rbx"。若栈帧中不存在该信息，则返回。
  // 同样处理rval，只不过寄存器改为rcx.
  // 生成指令add %rbx, %rcx
  // 从栈帧信息中查找result，如果不存在，则生成一条创建变量的指令。如果存在，则生成一条向栈写入的指令。
  std::string code = "";
  std::string str_lval = "";
  std::string str_rval = "";
  std::string str_op;
  if (!(op == Operator::OP_ADD || op == Operator::OP_SUB)) {
    error("invalid operator");
    return "";
  }
  if (result.tag != Tag::IDENTIFIER) {
    error("result must be identifier");
    return "";
  }
  code += gen_access_arg(lval, RBX);
  code += gen_access_arg(rval, RCX);
  if (op == Operator::OP_ADD) {
    str_op = "\taddq";
  } else {
    str_op = "\tsubq";
  }
  code += str_op + " %rcx, %rbx\n";
  if (curr_frame_.is_variable_exists(result.name)) {
    int offset = curr_frame_.get_variable_offset(result.name);
    if (offset == 0) {
      std::stringstream sstream;
      sstream << "var " << result.name << " offset is 0";
      print_error(sstream.str().c_str());
      return "";
    }
    code += gen_save_variable(RBX, offset);
  } else {
    code += gen_create_variable(RBX);
    curr_frame_.add_local(result.name, get_type_length(result.type));
  }
  return code;
}

/*
    生成进入过程时的代码
*/
std::string CodeGen::gen_enter_proc() {
  return "\tpushq %rbp\n"
         "\tmovq %rsp, %rbp\n";
}

/*
    生成退出过程时的代码，要将栈退出
*/
std::string CodeGen::gen_exit_proc() {
  /*
      退出临时分配的栈帧空间
  */
  std::stringstream sstream;
  sstream << ".end_" << function_->name << ":\n";
  sstream << "\tsubq %rsp, %rbp\n";
  sstream << "\taddq %rbp, %rsp\n";
  sstream << "\tpopq %rbp\n"
             "\tret\n";
  return sstream.str();
}

/*
    生成return返回值，将值赋给rax，
*/
std::string CodeGen::gen_return(const Var& result) {
  return gen_access_arg(result, RAX) + "\tjmp .end_" + function_->name + "\n";
}

/*
 * 生成一条从内存中载入变量到寄存器的指令
 * reg_name 寄存器名称 {"%rbx", "%rcx"}
 * offset 相对于bsp的偏移地址
 */
std::string CodeGen::gen_load_variable(const std::string& reg_name,
                                       int offset) {
  if (!register_check(reg_name)) {
    return "";
  }
  if (!frame_offset_check(reg_name, offset)) {
    return "";
  }
  std::stringstream sstream;
  sstream << "\tmovq " << offset << "(%rbp), " << reg_name << "\n";
  return sstream.str();
}

/*
 * 生成一条创建变量的汇编指令，实现方式是将变量压栈，类似 pushq %rbx
 */
std::string CodeGen::gen_create_variable(const std::string& reg_name) {
  if (!register_check(reg_name)) {
    return "";
  }
  std::stringstream sstream;
  sstream << "\tpushq " << reg_name << "\n";
  // sstream << "\taddq $8, %rdx\n";
  return sstream.str();
}

/*
    将变量保存在栈中
*/
std::string CodeGen::gen_save_variable(const std::string& reg_name,
                                       int offset) {
  if (!register_check(reg_name)) {
    return "";
  }
  if (!frame_offset_check(reg_name, offset)) {
    return "";
  }
  std::stringstream sstream;
  sstream << "\tmovq " << reg_name << ", " << offset << "(%rbp)\n";
  return sstream.str();
}

std::string CodeGen::gen_call(const Var& result, const std::string& fun_name) {
  if (fun_name == "print") {
    return gen_print_int(0);
  }
  std::stringstream sstream;
  sstream << "\tcall " << fun_name << "\n";
  if (result.tag == Tag::IDENTIFIER) {
    sstream << "\tmovq %rax, %rbx\n";
    sstream << gen_create_variable(RBX);
    curr_frame_.add_local(result.name, get_type_length(result.type));
  }
  return sstream.str();
}

std::string CodeGen::gen_set_param(const Var& param) {
  if (param.tag != Tag::LT_NUMBER && param.tag != Tag::IDENTIFIER) {
    std::string error_msg =
        "tag of param " + param.name + " must be LT_NUMBER or IDENTIFIER";
    error(error_msg.c_str());
    return "";
  }
  std::string code = "";
  code += gen_access_arg(param, RBX);
  code += "\tpushq %rbx\n";
  curr_frame_.add_param_out(get_type_length(param.type));
  return code;
}

/*
    生成语义为if arg1 op arg2 goto result的语句
*/
std::string CodeGen::gen_if_jump(Operator op, const Var& result,
                                 const Var& arg1, const Var& arg2) {
  /*
      步骤：
      1 载入arg1到rbx，arg2到rcx
      2 生成cmpq rbx, rcx
      3 根据操作符生成je，jge
  */
  std::string code = "";
  code += gen_access_arg(arg1, RBX);
  code += gen_access_arg(arg2, RCX);
  code += "\tcmpq " + RBX + ", " + RCX + "\n";
  std::string jump = "";
  switch (op) {
    case Operator::OP_LESS:
      jump = "jl";
      break;
    case Operator::OP_LESS_EQUAL:
      jump = "jle";
      break;
    case Operator::OP_EQUAL:
      jump = "je";
      break;
    case Operator::OP_NOT_EQUAL:
      jump = "jne";
      break;
    case Operator::OP_GREATER:
      jump = "jg";
      break;
    case Operator::OP_GREATER_EQUAL:
      jump = "jge";
      break;
    default:
      break;
  }
  std::string label = result.value_string;
  if (label.empty()) {
    error("label is empty");
    return "";
  }
  code += "\t" + jump + " " + label + "\n";
  return code;
}

std::string CodeGen::gen_if_false_jump(Operator op, const Var& result,
                                       const Var& arg1, const Var& arg2) {
  // 思路同gen_if_jump，除了跳转指令是反的
  std::string code = "";
  code += gen_access_arg(arg1, RBX);
  code += gen_access_arg(arg2, RCX);
  code += "\tcmpq " + RBX + ", " + RCX + "\n";
  std::string jump = "";
  switch (op) {
    case Operator::OP_LESS:
      jump = "jge";
      break;
    case Operator::OP_LESS_EQUAL:
      jump = "jg";
      break;
    case Operator::OP_EQUAL:
      jump = "jne";
      break;
    case Operator::OP_NOT_EQUAL:
      jump = "je";
      break;
    case Operator::OP_GREATER:
      jump = "jle";
      break;
    case Operator::OP_GREATER_EQUAL:
      jump = "jl";
      break;
    default:
      break;
  }
  std::string label = result.value_string;
  if (label.empty()) {
    error("label is empty");
    return "";
  }
  code += "\t" + jump + " " + label + "\n";
  return code;
}

std::string CodeGen::gen_jump(const Var& result) {
  std::string code = "";
  std::string label = result.value_string;
  if (label.empty()) {
    error("label is empty");
    return "";
  }
  code += "\tjmp " + label + "\n";
  return code;
  return std::string();
}

bool CodeGen::register_check(const std::string& reg_name) {
  if (reg_name == RBX || reg_name == RCX || reg_name == RAX) {
    return true;
  }
  std::string error_msg = "invalid register name:" + reg_name;
  error(error_msg.c_str());
  return false;
}

/*
 * 通过指令地址获取变量
 */
Var CodeGen::get_var(const Address& address) {
  switch (address.type) {
    case NAME:
    case TEMP_VAR:
      return function_->get_variable(address.value);
    case LITERAL_NUMBER:
      return Var(Tag::LT_NUMBER, "", address.value, Type::INT);
    case LITERAL_CHAR:
      return Var(Tag::LT_CHAR, "", address.value, Type::CHAR);
    case LITERAL_STRING:
      return Var(Tag::LT_STRING, "", address.value, Type::STRING);
    default:
      return Var();
  }
}

/**
    生成访问操作数的汇编代码，支持ID和整形常量
    var 操作数
    reg_name 寄存器名称 {"%rax", "%rbx", "%rcx"}
*/
std::string CodeGen::gen_access_arg(const Var& var,
                                    const std::string& reg_name) {
  std::string error_msg = "";
  if (!register_check(reg_name)) {
    return "";
  }
  std::string str_val;
  if (var.tag == Tag::LT_NUMBER) {
    str_val = "$" + var.value_string;
    return "\tmovq " + str_val + " , " + reg_name + "\n";
  } else if (var.tag == Tag::IDENTIFIER) {
    if (!variable_exist_check(var.name)) {
      return "";
    }
    int offset = curr_frame_.get_variable_offset(var.name);
    if (!frame_offset_check(var.name, offset)) {
      return "";
    }
    return gen_load_variable(reg_name, offset);
  }
  return "";
}

/*
    将每个参数的偏移地址存储到栈帧当中，以便后续读取。
    因为函数开始执行之前调用过call(本质就是push rip)和push rbp，
    所以参数的偏移起始地址为rbp+16。
*/
void CodeGen::parse_params() {
  assert(function_ != nullptr);
  int offset = PARAM_OFFSET_BEGIN;
  const std::vector<Var> params = function_->get_params();
  for (unsigned int i = 0; i < params.size(); i++) {
    Var var = params[i];
    curr_frame_.add_param_in(var.name, offset);
    int length = get_type_length(var.type);
    offset += length;
  }
}

/*
    获取类型的长度，单位为字节
*/
int CodeGen::get_type_length(const Type& type) {
  switch (type) {
    case Type::INT:
      return 8;
  }
  return 0;
}

bool CodeGen::variable_exist_check(const std::string& var_name) {
  if (!curr_frame_.is_variable_exists(var_name)) {
    std::string error_msg = "var " + var_name + " not found in stackframe";
    error(error_msg.c_str());
    return false;
  }
  return true;
}

bool CodeGen::frame_offset_check(const std::string& var_name, int offset) {
  if (offset >= LOCAL_OFFSET_BEGIN && offset < PARAM_OFFSET_BEGIN) {
    std::stringstream sstream;
    sstream << "var " << var_name << " offset is " << offset
            << ",it can't be in [0,16)";
    error(sstream.str().c_str());
    return false;
  }
  return true;
}