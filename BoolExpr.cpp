#include "BoolExpr.h"

#include <iostream>
#include <sstream>
#include <string>

void BoolExpr::back_patch(Function* function, BackPatchType type,
                          instr_number label) {
  if (type == BackPatchType::True) {
    back_patch_list(function, true_list, label);
  } else if (type == BackPatchType::False) {
    back_patch_list(function, false_list, label);
  }
}

void BoolExpr::back_patch_list(Function* function,
                               const std::vector<instr_number>& list,
                               instr_number label) {
  std::stringstream sstream;
  sstream << label;
  std::string str = sstream.str();
  for (auto iter = list.begin(); iter != list.end(); ++iter) {
    InterInstruction& inst = function->get_instruction(*iter);
    Address addr = Address{LITERAL_STRING, str};
    inst.result = addr;
  }
}
