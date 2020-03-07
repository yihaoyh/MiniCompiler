#pragma once
#include"Param.h"
#include<vector>
#include<map>
#include<string>
class Frame
{
public:
	const Frame* parent;
	std::vector<Param> param_list;
	std::string fun_name;
	Frame()
	{

	}

	Frame(const Frame* parent, const std::string& fun_name, const std::vector<Param>& param_list) :parent(parent), fun_name{fun_name}
	{
		this->param_list = param_list;
	}

	void insert_variable(std::string name, unsigned char length);
	bool is_variable_exists(std::string name);
	long get_variable_offset(std::string name);
private:
	std::map<std::string, long> var_offset_table;       // 变量偏移表
	long frame_offset_ = 0;                        // 相对bsp的偏移
};

