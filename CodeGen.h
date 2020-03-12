#pragma once
#include<vector>
#include<string>
#include"InterInstruction.h"
#include"Frame.h"
#include"SymTable.h"
#include"Function.h"
/*
 * 汇编生成过程：
 * 1 解析所有函数的参数信息和返回值信息
 * 2 对于函数列表中的每个函数：
 *	2.1 创建栈帧
 *	2.2 生成头部代码，包括段，标签等信息。
 *  2.3 生成进入栈帧代码
 *	2.4 对于函数的指令列表中的每条指令：
 *		2.4.1 翻译成相应的代码
 *  2.5 生成退出栈帧代码
 * 
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
		//curr_frame_ = Frame(nullptr, std::string("main"), std::vector<Var>());
		//frame_stack_.push_back(curr_frame_);
	}
	std::string parse_functions(std::vector<Function> functions);
	std::string parse_function(std::vector<InterInstruction> insts);
private:
	std::string parse_instruction(const InterInstruction& inst);
	std::string gen_header(const std::string& fun_name);
    std::string gen_assign(const Var& lval, const Var& rval);
	std::string gen_low_op(Operator op, const Var& result, const Var& lval, const Var& rval);
	std::string gen_sub(const Var& result, const Var& lval, const Var& rval);
	std::string gen_mul(const Var& result, const Var& lval, const Var& rval);
	std::string gen_div(const Var& result, const Var& lval, const Var& rval);
	std::string gen_enter_proc();
	std::string gen_exit_proc();
	std::string gen_return(const Var& result);
	std::string gen_load_variable(const std::string& reg_name, long offset);
	std::string gen_create_variable(const std::string& reg_name);
	std::string gen_save_variable(const std::string& reg_name, long offset);
	std::string gen_call(const Var& result, const std::string& fun_name);
	std::string gen_set_param(const Var& param);
	bool is_reg_valid(const std::string& reg_name);
	Var get_var(const Address& address);
	std::string gen_access_arg(const Var& var, const std::string& reg_name);
	void parse_params();
	int get_type_length(const Type& type);

	//std::vector<Frame> frame_stack_;
	Frame curr_frame_;

	// TODO 符号表
	//SymTable* symtable_;
	Function* function_ = nullptr;
};

