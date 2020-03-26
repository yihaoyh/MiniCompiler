#pragma once
#include <map>
#include <string>
#include <vector>

#include "Var.h"
#include "InterInstruction.h"
#include "TypeExpr.h"

class SymTable {
 public:
  void put_variable(Var);
  Var get_variable(std::string);
  Var get_const_variable(std::string) const;
  unsigned int get_next_instruction();
  Var gen_temp_var(const TypeExpr& t_expr);  // 生成临时变量
  const std::vector<Var> get_variable_list() const;
 private:
  std::map<std::string, Var> var_table_;      // 变量表
  std::map<std::string, Var> temp_var_table;  // 临时变量表
  std::vector<InterInstruction> inst_vector;
};