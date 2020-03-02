#pragma once
#include"Param.h"
#include<vector>
class Frame
{
public:
	Frame& parent;
	std::vector<Param> param_list;
	Frame(Frame parent, std::vector<Param> param_list):parent(parent)
	{
		this->param_list = param_list;
	}
};

