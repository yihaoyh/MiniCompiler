#pragma once
#include <string>
#include <vector>
#include <map>
#include "InterInstruction.h"
#include "SymTable.h"
#include "Var.h"
class Function {
 public:
  Function(const std::string& fun_name, bool is_declaration, const TypeExpr& t_epxr,
           const std::vector<Var>& params);
  Function();
  unsigned int add_instruction(InterInstruction* inst);
  InterInstruction& get_instruction(unsigned int index);
  std::vector<InterInstruction>& get_instructions();
  const std::vector<InterInstruction>& get_const_instructions() const;
  const std::vector<Var> get_params() const;
  instr_number get_next_instruction();
  void put_variable(const Var& var);
  const Var get_variable(const std::string& name) const;
  const std::vector<Var> get_variable_list() const;
  Var gen_temp_var(const TypeExpr t_expr);
  Var gen_temp_pointer(const TypeExpr type);
  std::string gen_label();

  bool is_declaration;  // true表示声明，false表示定义
  std::string name;     // 函数名称
  TypeExpr return_type = UNKNOWN_TYPE_EXPR;

 private:
  SymTable sym_table_;
  std::vector<Var> params_;
  std::vector<InterInstruction> inst_vector_;
  unsigned int label_index_ = 0;
};
