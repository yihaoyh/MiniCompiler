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
 * ����һ�����ڴ�������������Ĵ�����ָ��
 * reg_name �Ĵ������ƣ�����ֻ֧��rbx����rcx
 * offset �����bsp��ƫ�Ƶ�ַ
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
 * ����һ�����������Ļ��ָ�ʵ�ַ�ʽ�ǽ�����ѹջ������ pushq %ebx
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

