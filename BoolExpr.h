#pragma once
#include<string>
#include<vector>
#include"InterInstruction.h"
class BoolExpr : public InterInstruction
{
public:
    std::string true_label;
    std::string false_label;
    std::vector<unsigned int> true_list;
    std::vector<unsigned int> false_list;
};

