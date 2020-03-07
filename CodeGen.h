#pragma once
#include<vector>
#include<string>
#include"InterInstruction.h"
#include"Frame.h"
/*
 * 汇编生成规则
 * 1 最多使用两个寄存器，bx和cx
 * 2 若变量在栈帧中不存在，则先用bx完成操作，然后压栈，并将偏移地址记录在栈帧信息中。若存在，则从栈帧信息中读取偏移地址，然后
 *   将其读至rbx或者rcx中，优先使用rbx。
 * 3 退出栈帧时，要将栈帧信息中的变量全部退出。
 */
class CodeGen
{
public:
	CodeGen() 
	{
		frame_ = Frame(nullptr, std::string("main"), std::vector<Param>());
	}
	std::string begin(std::vector<InterInstruction> insts);
private:
	std::string parse_instruction(const InterInstruction& inst);
	std::string gen_header(const std::string& fun_name);
    std::string gen_assign(const Var& lval, const Var& rval);
	std::string gen_low_op(Operator op, const Var& result, const Var& lval, const Var& rval);
	std::string gen_sub(const Var& result, const Var& lval, const Var& rval);
	std::string gen_mul(const Var& result, const Var& lval, const Var& rval);
	std::string gen_div(const Var& result, const Var& lval, const Var& rval);
	std::string gen_enter_call();
	std::string gen_exit_call();
	std::string gen_load_variable(const std::string reg_name, long offset);
	std::string gen_create_variable(const std::string reg_name);
	std::string gen_save_variable(const std::string reg_name, long offset);
	bool is_reg_valid(std::string reg_name);

	Frame frame_;
};

