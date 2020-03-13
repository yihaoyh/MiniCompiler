#pragma once
#include<vector>
#include<string>
#include"InterInstruction.h"
#include"Frame.h"
#include"SymTable.h"
#include"Function.h"
/*
 * ������ɹ��̣�
 * 1 �������к����Ĳ�����Ϣ�ͷ���ֵ��Ϣ
 * 2 ���ں����б��е�ÿ��������
 *	2.1 ����ջ֡
 *	2.2 ����ͷ�����룬�����Σ���ǩ����Ϣ��
 *  2.3 ���ɽ���ջ֡����
 *	2.4 ���ں�����ָ���б��е�ÿ��ָ�
 *		2.4.1 �������Ӧ�Ĵ���
 *  2.5 �����˳�ջ֡����
 * 
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
	}
	std::string parse_functions(std::vector<Function> functions);
private:
	std::string parse_function(Function& fun);
	std::string parse_instruction(const InterInstruction& inst);
	std::string gen_header(const std::string& fun_name);
    std::string gen_assign(const Var& lval, const Var& rval);
	std::string gen_low_op(Operator op, const Var& result, const Var& lval, const Var& rval);
	std::string gen_enter_proc();
	std::string gen_exit_proc();
	std::string gen_return(const Var& result);
	std::string gen_load_variable(const std::string& reg_name, long offset);
	std::string gen_create_variable(const std::string& reg_name);
	std::string gen_save_variable(const std::string& reg_name, long offset);
	std::string gen_call(const Var& result, const std::string& fun_name);
	std::string gen_set_param(const Var& param);
	bool register_check(const std::string& reg_name);
	bool frame_offset_check(const std::string& var_name, long offset);
	bool variable_exist_check(const std::string& var_name);
	Var get_var(const Address& address);
	
	/**
	���ɷ��ʲ������Ĵ��룬֧��ID�����γ���
	@param[in] var ������
	reg_name
	*/	
	std::string gen_access_arg(const Var& var, const std::string& reg_name);
	void parse_params();
	int get_type_length(const Type& type);

	//std::vector<Frame> frame_stack_;
	Frame curr_frame_;

	// TODO ���ű�
	//SymTable* symtable_;
	Function* function_ = nullptr;
};

