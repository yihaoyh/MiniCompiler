#pragma once
#include <string>
#include <vector>

#include "Common.h"
#include "Function.h"
#include "InterInstruction.h"

enum class BackPatchType { True, False };

class BoolExpr {
 public:
  /*
      回填跳转列表
      function 表示当前函数
      type 跳转类型
      label表示目标标号
  */
  void back_patch(Function* function, BackPatchType type, instr_number label);
  static void back_patch_list(Function* function,
                              const std::vector<instr_number>& list,
                              instr_number label);

  std::string true_label;
  std::string false_label;
  std::vector<instr_number> true_list;
  std::vector<instr_number> false_list;
};
