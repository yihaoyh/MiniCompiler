#pragma once
#include<vector>
#include<string>
#include"InterInstruction.h"
#include"Frame.h"
/*
 * ������ɹ���
 * 1 ���ʹ�������Ĵ�����bx��cx
 * 2 ��������ջ֡�в����ڣ�������bx��ɲ�����Ȼ��ѹջ������ƫ�Ƶ�ַ��¼��ջ֡��Ϣ�С������ڣ����ջ֡��Ϣ�ж�ȡƫ�Ƶ�ַ��Ȼ��
 *   �������rbx����rcx�У�����ʹ��rbx��
 * 3 �˳�ջ֡ʱ��Ҫ��ջ֡��Ϣ�еı���ȫ���˳���
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

