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
 * ���ɸ�ֵָ�Ϊ���ٹ�������ֻ֧��64λ����
 * lval ��ֵ����rbx�Ĵ���
 * rval ��ֵ����rcx�Ĵ�������������
 * �ؼ�ָ����ʽ���� mov %rcx, %rbx ���� mov $10, %rbx
*/
std::string CodeGen::gen_assign(const Var& lval, const Var& rval)
{
	// �ж�rval�Ǳ�������������
	// ������������������������ַ�����ʽ�� str_rval = "$10"
	// ����Ǳ��������жϸñ�����ջ֡��Ϣ���Ƿ���ڡ�
	// ������ڣ�������һ��ָ���ָ����ݱ�����ƫ�Ƶ�ַ�����뵽ecx�Ĵ����У���ʹstr_rval = "%rcx"
	// ��������ڣ��򷵻�
	// ����ָ�� movq $str_rval, %rbx
	// Ȼ���ж�lval�Ƿ������ջ֡��Ϣ�У�������ڣ�������ָ�rbxд�뵽��Ӧ�ڴ��С�
	// ��������ڣ�������ָ�� push rbx��������Frame::insert_variable��ջ֡��Ϣ���һ������
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
 * ���ɼ�/����ָ�Ϊ���ٹ�������ֻ֧��64λ����
 * result ���������Ϊ����
 * lval ��ֵ
 * rval ��ֵ
 */
std::string CodeGen::gen_low_op(Operator op, const Var& result, const Var& lval, const Var& rval)
{
	// �ж�result��tag�Ƿ�ΪID�������ǣ��򷵻�
	// ���lval����������������str_lvalΪ"$number"
	// �������ջ֡��Ϣ������һ��ָ���ָ����ڴ��ж�ȡlval��ֵ��rbx, str_lval����Ϊ"%rbx"����ջ֡�в����ڸ���Ϣ���򷵻ء�
	// ͬ������rval��ֻ�����Ĵ�����Ϊrcx.
	// ����ָ��add %rbx, %rcx
	// ��ջ֡��Ϣ�в���result����������ڣ�������һ������������ָ�������ڣ�������һ����ջд���ָ�
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
 * ����һ�����ڴ�������������Ĵ�����ָ��
 * reg_name �Ĵ������ƣ�����ֻ֧��rbx����rcx
 * offset �����bsp��ƫ�Ƶ�ַ
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
 * ����һ�����������Ļ��ָ�ʵ�ַ�ʽ�ǽ�����ѹջ������ pushq %rbx
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
 * ͨ��ָ���ַ��ȡ����
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
	int offset = 8; // ��Ϊ֮ǰִ�й�push rbp��mov rsp, rbp������ƫ����ʼλ8
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
	��ȡ���͵ĳ��ȣ���λΪ�ֽ�
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
