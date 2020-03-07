#include "CodeGen.h"
#include <sstream>
#include "Common.h"
#define VAR_LENGTH 8
std::string CodeGen::begin(std::vector<InterInstruction> insts)
{
	std::string code = "";
	code += gen_header(frame_.fun_name);
	code += gen_enter_call();
	for (auto iter = insts.begin(); iter != insts.end(); ++iter)
	{
		code += parse_instruction(*iter);
	}
	return code;
}

std::string CodeGen::parse_instruction(const InterInstruction& inst)
{
	switch (inst.op)
	{
	case Operator::OP_ADD:
	case Operator::OP_SUB:
		return gen_low_op(inst.op, inst.result, inst.arg1, inst.arg2);
	case Operator::OP_ASSIGN:
		return gen_assign(inst.result, inst.arg1);
	default:
		return "";
	}
}

std::string CodeGen::gen_header(const std::string& fun_name)
{
	if (fun_name.empty())
	{
		return "";
	}
	std::stringstream sstream;
	sstream << "\t.globl  " << fun_name << "\n";
	sstream << fun_name << ":" << "\n";
	return sstream.str();
}

/*
 * 生成赋值指令，为减少工作量，只支持64位整形
 * lval 左值，用rbx寄存器
 * rval 右值，用rcx寄存器或者立即数
 * 关键指令形式类似 mov %rcx, %rbx 或者 mov $10, %rbx
*/
std::string CodeGen::gen_assign(const Var& lval, const Var& rval)
{
	// 判断rval是变量还是字面量
	// 如果是字面量，生成字面量字符串形式如 str_rval = "$10"
	// 如果是变量，则判断该变量在栈帧信息中是否存在。
	// 如果存在，则生成一条指令，该指令根据变量的偏移地址，读入到ecx寄存器中，并使str_rval = "%rcx"
	// 如果不存在，则返回
	// 生成指令 movq $str_rval, %rbx
	// 然后判断lval是否存在于栈帧信息中，如果存在，则生成指令将rbx写入到相应内存中。
	// 如果不存在，则生成指令 push rbx，并调用Frame::insert_variable向栈帧信息添加一个变量
	std::string code = "";
	std::string str_rval = "";
	if (lval.tag != IDENTIFIER)
	{
		return "";
	}
	if (rval.tag == LT_NUMBER)
	{
		str_rval = "$" + rval.value_string;
	}
	else if (rval.tag == IDENTIFIER)
	{
		if (!frame_.is_variable_exists(rval.name))
		{
			std::string str_error = "var " + rval.name + " not exists in frame";
			print_error(str_error.c_str());
			return "";
		}
		long offset = frame_.get_variable_offset(rval.name);
		if (offset == 0)
		{
			std::stringstream sstream;
			sstream << "var " << rval.name << " offset is 0";
			print_error(sstream.str().c_str());
			return "";
		}
		code += gen_load_variable("%rcx", offset);
		str_rval = "%rcx";
	}
	code += "movq " + str_rval + ", %rbx\n";
	if (frame_.is_variable_exists(lval.name))
	{
		long offset = frame_.get_variable_offset(lval.name);
		if (offset == 0)
		{
			std::stringstream sstream;
			sstream << "var " << lval.name << " offset is 0";
			print_error(sstream.str().c_str());
			return "";
		}
		code += gen_save_variable("%rbx", offset);
	}
	else
	{
		code += gen_create_variable("%rbx");
		frame_.insert_variable(lval.name, 8);
	}
	return code;
}

/*
 * 生成加/减法指令，为减少工作量，只支持64位整形
 * result 结果，必须为变量
 * lval 左值
 * rval 右值
 */
std::string CodeGen::gen_low_op(Operator op, const Var& result, const Var& lval, const Var& rval)
{
	// 判断result的tag是否为ID，若不是，则返回
	// 如果lval是整形字面量，则str_lval为"$number"
	// 否则根据栈帧信息，生成一条指令，该指令从内存中读取lval的值到rbx, str_lval设置为"%rbx"。若栈帧中不存在该信息，则返回。
	// 同样处理rval，只不过寄存器改为rcx.
	// 生成指令add %rbx, %rcx
	// 从栈帧信息中查找result，如果不存在，则生成一条创建变量的指令。如果存在，则生成一条向栈写入的指令。
	std::string code = "";
	std::string str_lval = "";
	std::string str_rval = "";
	std::string str_op;
	if (!(op == Operator::OP_ADD||op == Operator::OP_SUB))
	{
		return "";
	}
	if (result.tag != IDENTIFIER)
	{
		return "";
	}
	if (lval.tag == LT_NUMBER)
	{
		str_lval = "$" + lval.value_string;
		code += "movq " + str_lval + " , %rbx\n";
	}
	else if (lval.tag == IDENTIFIER)
	{
		if (!frame_.is_variable_exists(lval.name))
		{

			return "";
		}
		long offset = frame_.get_variable_offset(lval.name);
		if (offset == 0)
		{
			std::stringstream sstream;
			sstream << "var " << lval.name << " offset is 0";
			print_error(sstream.str().c_str());
			return "";
		}
		code += gen_load_variable("%rbx", offset);
		str_lval = "%rbx";
	}
	if (rval.tag == LT_NUMBER)
	{
		str_rval = "$" + rval.value_string;
		code += "movq " + str_rval + " , %rcx\n";
	}
	else if (rval.tag == IDENTIFIER)
	{
		if (!frame_.is_variable_exists(rval.name))
		{
			return "";
		}
		long offset = frame_.get_variable_offset(rval.name);
		if (offset == 0)
		{
			std::stringstream sstream;
			sstream << "var " << rval.name << " offset is 0";
			print_error(sstream.str().c_str());
			return "";
		}
		code += gen_load_variable("%rcx", offset);
		str_rval = "%rcx";
	}
	if(op == Operator::OP_ADD)
	{ 
		str_op = "addq";
	}
	else
	{
		str_op = "subq";
	}
	code += str_op + " %rcx, %rbx\n";
	if (frame_.is_variable_exists(result.name))
	{
		long offset = frame_.get_variable_offset(result.name);
		if (offset == 0)
		{
			std::stringstream sstream;
			sstream << "var " << result.name << " offset is 0";
			print_error(sstream.str().c_str());
			return "";
		}
		code += gen_save_variable("%rbx", offset);
	}
	else
	{
		code += gen_create_variable("%rbx");
		frame_.insert_variable(result.name, 8);
	}
	return code;
}

std::string CodeGen::gen_sub(const Var& result, const Var& lval, const Var& rval)
{
	return std::string();
}

std::string CodeGen::gen_mul(const Var& result, const Var& lval, const Var& rval)
{
	return std::string();
}

std::string CodeGen::gen_div(const Var& result, const Var& lval, const Var& rval)
{
	return std::string();
}

std::string CodeGen::gen_enter_call()
{
	
	return "pushq %rbp\nmovq %rsp, %rbp\n";
}

std::string CodeGen::gen_exit_call()
{
	std::string code = "popq %rbp\n";
	code += "ret\n";
	return code;
}

/*
 * 生成一条从内存中载入变量到寄存器的指令
 * reg_name 寄存器名称，现在只支持rbx或者rcx
 * offset 相对于bsp的偏移地址
 */
std::string CodeGen::gen_load_variable(const std::string reg_name, long offset)
{
	if (!is_reg_valid(reg_name))
	{
		return "";
	}
	std::stringstream sstream;
	if (offset >= 0)
	{
		return "";
	}
	sstream << "movq " << offset << "(%rbp), " << reg_name << "\n";
	return sstream.str();
}

/*
 * 生成一条创建变量的汇编指令，实现方式是将变量压栈，类似 pushq %ebx
 */
std::string CodeGen::gen_create_variable(const std::string reg_name)
{
	if (!is_reg_valid(reg_name))
	{
		return "";
	}
	std::stringstream sstream;
	sstream << "pushq " << reg_name << "\n";
	return sstream.str();
}

std::string CodeGen::gen_save_variable(const std::string reg_name, long offset)
{
	if (!is_reg_valid(reg_name))
	{
		return "";
	}
	std::stringstream sstream;
	sstream << "movq " << reg_name << ", " << offset << "(%rbp)\n";
	return sstream.str();
}

bool CodeGen::is_reg_valid(std::string reg_name)
{
	if (reg_name == "%rbx" || reg_name == "%rcx")
	{
		return true;
	}
	return false;
}

