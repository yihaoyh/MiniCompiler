#pragma once
#include<string>
#include<vector>
#include"InterInstruction.h"
#include"Function.h"
#include"Common.h"
class BoolExpr
{
public:
    /* 
        回填跳转列表
        function 表示当前函数
        flag 1对应true_list,0对应false_list
        label表示目标标号
    */
    void back_patch(Function& function, int flag, instr_index label);
    static void back_patch_list(Function& function, std::vector<instr_index> list, instr_index label);

    std::string true_label;
    std::string false_label;
    std::vector<instr_index> true_list;
    std::vector<instr_index> false_list;
};

