#pragma once
#include"Param.h"
#include<vector>
#include<map>
#include<string>
class Frame
{
public:
	const Frame* parent;
	std::vector<Var> param_list;
	std::string fun_name;
	Frame()
	{
		parent = nullptr;
	}

	Frame(const Frame* parent, const std::string& fun_name, const std::vector<Var>& param_list) :parent(parent), fun_name{fun_name}
	{
		this->param_list = param_list;
	}

	void add_local(std::string name, unsigned char length);   // ��Ӿֲ�����
	void add_param_in(std::string name, char offset);            // ��Ӳ���
	void add_param_out(unsigned char length);
	bool is_variable_exists(std::string name);
	long get_variable_offset(std::string name);
	long get_frame_length();
private:
	std::map<std::string, long> var_offset_table;       // ����ƫ�Ʊ�
	long frame_offset_ = 0;                        // ���bsp��ƫ��
};

