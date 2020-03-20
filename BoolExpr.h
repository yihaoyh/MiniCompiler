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
        ������ת�б�
        function ��ʾ��ǰ����
        flag 1��Ӧtrue_list,0��Ӧfalse_list
        label��ʾĿ����
    */
    void back_patch(Function& function, int flag, instr_index label);
    static void back_patch_list(Function& function, std::vector<instr_index> list, instr_index label);

    std::string true_label;
    std::string false_label;
    std::vector<instr_index> true_list;
    std::vector<instr_index> false_list;
};

