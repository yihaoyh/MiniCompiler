#include "BoolExpr.h"
#include <sstream>
void BoolExpr::back_patch(Function& function, int flag, instr_number label)
{
    if (flag == 0)
    {
        back_patch_list(function, false_list, label);
    }
    else if (flag == 1)
    {
        back_patch_list(function, true_list, label);
    }
}

void BoolExpr::back_patch_list(Function& function, std::vector<instr_number> list, instr_number label)
{
    std::stringstream sstream;
    sstream << label;
    std::string str = sstream.str();
    for (auto iter = list.begin(); iter != list.end(); ++iter)
    {
        InterInstruction& inst = function.get_instruction(*iter);
        Address addr = Address{LITERAL_STRING, str};
        inst.result = addr;
    }
}

