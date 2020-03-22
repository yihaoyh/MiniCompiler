#pragma once
#include <string>
#include <vector>

#include "Common.h"
#include "Function.h"
#include "InterInstruction.h"
class BoolExpr {
 public:
  /*
      回填跳转列表
      function 表示当前函数
      flag 1对应true_list,0对应false_list
      label表示目标标号
  */
  void back_patch(Function* function, int flag, instr_number label);
  static void back_patch_list(Function* function,
                              const std::vector<instr_number>& list,
                              instr_number label);

  std::string true_label;
  std::string false_label;
  std::vector<instr_number> true_list;
  std::vector<instr_number> false_list;
};
