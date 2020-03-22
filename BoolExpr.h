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
    void back_patch(Function& function, int flag, instr_number label);
    static void back_patch_list(Function& function, std::vector<instr_number> list, instr_number label);

    std::string true_label;
    std::string false_label;
    std::vector<instr_number> true_list;
    std::vector<instr_number> false_list;
};

