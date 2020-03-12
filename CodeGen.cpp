#include "CodeGen.h"
#include <sstream>
#include "Common.h"
#include "Utils.h"

#define VAR_LENGTH 8
std::string CodeGen::parse_function(std::vector<InterInstruction> insts)
{
	std::string code = "";
	code += gen_header(curr_frame_.fun_name);
	code += gen_enter_proc();
	for (auto iter = insts.begin(); iter != insts.end(); ++iter)
	{
		code += parse_instruction(*iter);
	}
	code += gen_exit_proc();
	return code;
}

std::string CodeGen::parse_functions(std::vector<Function> functions)
{
	std::string code = "";
	for (auto iter = functions.begin(); iter != functions.end(); ++iter)
	{
		function_ = &(*iter);
		curr_frame_ = Frame(nullptr, iter->name, iter->get_params());
		parse_params();
		code += parse_function(function_->get_instructions());
	}
	return code;
}

std::string CodeGen::parse_instruction(const InterInstruction& inst)
{
	Var result = get_var(inst.result);
	Var arg1 = get_var(inst.arg1);
	Var arg2 = get_var(inst.arg2);
	std::string code = "";
	switch (inst.op)
	{
	case Operator::OP_ADD:
	case Operator::OP_SUB:
		if (result.tag != IDENTIFIER)
		{
			error("result must be lvalue in add/sub");
			return "";
		}
		if (arg1.tag!=IDENTIFIER && arg1.tag != LT_NUMBER)
		{
			error("arg1 must be number or id in add/sub");
		}
		if (arg2.tag != IDENTIFIER && arg2.tag != LT_NUMBER)
		{
			error("arg2 must be number or id in add/sub");
		}
		return gen_low_op(inst.op, result, arg1, arg2);
	case Operator::OP_ASSIGN:
		if (result.tag != IDENTIFIER)
		{
			error("result must be lvalue in add/sub");
			return "";
		}
		return gen_assign(result, arg1);
	case Operator::OP_SET_PARAM:
		return gen_set_param(arg1);
	case Operator::OP_CALL:
		code = gen_call(result, inst.arg1.value);
		return code;
	case Operator::OP_RETURN:
		return gen_return(arg1);
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
	if (fun_name == "main")
	{
		sstream << ".LC0:\n" \
			"\t.string	\"%d\\n\"\n"\
			"\t.text\n";
	}
	else
	{
		sstream << "\t.text\n";
	}
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
		if (!curr_frame_.is_variable_exists(rval.name))
		{
			std::string str_error = "var " + rval.name + " not exists in frame";
			print_error(str_error.c_str());
			return "";
		}
		long offset = curr_frame_.get_variable_offset(rval.name);
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
	if (curr_frame_.is_variable_exists(lval.name))
	{
		long offset = curr_frame_.get_variable_offset(lval.name);
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
		curr_frame_.add_local(lval.name, 8);
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
	if (!(op == Operator::OP_ADD || op == Operator::OP_SUB))
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
		if (!curr_frame_.is_variable_exists(lval.name))
		{
			return "";
		}
		long offset = curr_frame_.get_variable_offset(lval.name);
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
		if (!curr_frame_.is_variable_exists(rval.name))
		{
			return "";
		}
		long offset = curr_frame_.get_variable_offset(rval.name);
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
	if (op == Operator::OP_ADD)
	{
		str_op = "addq";
	}
	else
	{
		str_op = "subq";
	}
	code += str_op + " %rcx, %rbx\n";
	if (curr_frame_.is_variable_exists(result.name))
	{
		long offset = curr_frame_.get_variable_offset(result.name);
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
		curr_frame_.add_local(result.name, 8);
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

std::string CodeGen::gen_enter_proc()
{
	return "pushq %rbp\n"\
		   "movq %rsp, %rbp\n";
}

std::string CodeGen::gen_exit_proc()
{
	std::stringstream sstream;
	// addq $16, %rsp
	sstream << "addq $" << curr_frame_.get_frame_length() << ", %rsp\n";
	sstream << "popq %rbp\n"\
		    "ret\n";
	return sstream.str();
}

std::string CodeGen::gen_return(const Var& result)
{
	std::string code = "";
	code += gen_access_arg(result, "%rbx");
	code += "movq %rbx, %rax\n";
	return code;
}

/*
 * 生成一条从内存中载入变量到寄存器的指令
 * reg_name 寄存器名称，现在只支持rbx或者rcx
 * offset 相对于bsp的偏移地址
 */
std::string CodeGen::gen_load_variable(const std::string& reg_name, long offset)
{
	if (!is_reg_valid(reg_name))
	{
		return "";
	}
	std::stringstream sstream;
	sstream << "movq " << offset << "(%rbp), " << reg_name << "\n";
	return sstream.str();
}

/*
 * 生成一条创建变量的汇编指令，实现方式是将变量压栈，类似 pushq %rbx
 */
std::string CodeGen::gen_create_variable(const std::string& reg_name)
{
	if (!is_reg_valid(reg_name))
	{
		return "";
	}
	std::stringstream sstream;
	sstream << "pushq " << reg_name << "\n";
	return sstream.str();
}

std::string CodeGen::gen_save_variable(const std::string& reg_name, long offset)
{
	if (!is_reg_valid(reg_name))
	{
		return "";
	}
	std::stringstream sstream;
	sstream << "movq " << reg_name << ", " << offset << "(%rbp)\n";
	return sstream.str();
}

std::string CodeGen::gen_call(const Var& result, const std::string& fun_name)
{
	std::stringstream sstream;
	sstream << "call " << fun_name << "\n";
	if (result.tag != UNKNOWN)
	{
		sstream << "movq %rax, %rbx\n";
		sstream << gen_create_variable("%rbx");
		curr_frame_.add_local(result.name, get_type_length(result.type));
	}
	return sstream.str();
}

std::string CodeGen::gen_set_param(const Var& param)
{
	std::string code = "";
	code += gen_access_arg(param, "%rbx");
	code += "pushq %rbx\n";
	curr_frame_.add_param_out(get_type_length(param.type));
	return code;
}

bool CodeGen::is_reg_valid(const std::string& reg_name)
{
	if (reg_name == "%rbx" || reg_name == "%rcx")
	{
		return true;
	}
	return false;
}

/*
 * 通过指令地址获取变量
 */
Var CodeGen::get_var(const Address& address)
{
	switch (address.type)
	{
	case NAME:
	case TEMP_VAR:
		return function_->get_variable(address.value);
	case LITERAL_NUMBER:
		return Var(LT_NUMBER, "", address.value, Type::INT);
	case LITERAL_CHAR:
		return Var(LT_CHAR, "", address.value, Type::CHAR);
	case LITERAL_STRING:
		return Var(LT_STRING, "", address.value, Type::STRING);
	default:
		return Var();
	}
}

std::string CodeGen::gen_access_arg(const Var& var, const std::string& reg_name)
{
	if (!is_reg_valid(reg_name))
	{
		return "";
	}
	std::string str_val;
	if (var.tag == LT_NUMBER)
	{
		str_val = "$" + var.value_string;
		return "movq " + str_val + " , " + reg_name + "\n";
	}
	else if (var.tag == IDENTIFIER)
	{
		if (!curr_frame_.is_variable_exists(var.name))
		{
			return "";
		}
		long offset = curr_frame_.get_variable_offset(var.name);
		if (offset == 0)
		{
			std::stringstream sstream;
			sstream << "var " << var.name << " offset is 0";
			print_error(sstream.str().c_str());
			return "";
		}
		return gen_load_variable(reg_name, offset);
	}
	return "";
}

void CodeGen::parse_params()
{
	if (function_ == nullptr)
	{
		return ;
	}
	std::string code = "";
	int offset = 8; // 因为之前执行过push rbp和mov rsp, rbp，所以偏移起始位8
	const std::vector<Var> params = function_->get_params();
	for (unsigned int i = 0; i < params.size(); i++)
	{
		Var var = params[i];
		int length = get_type_length(var.type);
		if (length == 0)
		{
			continue;
		}
		offset += length;
		curr_frame_.add_param_in(var.name, offset);
	}
}

/* 
	获取类型的长度，单位为字节
*/
int CodeGen::get_type_length(const Type& type)
{
	switch (type)
	{
	case Type::INT:
		return 8;
	}
	return 0;
}
