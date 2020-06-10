#include "BoolExpr.h"
#include <assert.h>

#include <iostream>
#include <sstream>
#include <string>

void BoolExpr::back_patch(Function* function, BackPatchType type,
                          instr_number label) {
  assert(function != nullptr);
  if (type == BackPatchType::True) {
    BoolExpr::back_patch_list(function, true_list, label);
  } else if (type == BackPatchType::False) {
    BoolExpr::back_patch_list(function, false_list, label);
  }
}

void BoolExpr::back_patch_list(Function* function,
                               const std::vector<instr_number>& list,
                               instr_number label) {
  for (auto iter = list.begin(); iter != list.end(); ++iter) {
    InterInstruction& inst = function->get_instruction(*iter);
    inst.result = Address::label_addr(label);
  }
}
