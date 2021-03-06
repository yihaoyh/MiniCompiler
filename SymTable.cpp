﻿#include "SymTable.h"

#include <sstream>
void SymTable::put_variable(Var var) { var_table_[var.name] = var; }

Var SymTable::get_variable(std::string name) {
  if (var_table_.find(name) != var_table_.end()) {
    return var_table_[name];
  }
  return Var::default_;
}

Var SymTable::get_const_variable(std::string name) const {
  if (var_table_.find(name) != var_table_.end()) {
    return (*var_table_.find(name)).second;
  }
  return Var::default_;
}

unsigned int SymTable::get_next_instruction() { return inst_vector.size(); }

Var SymTable::gen_temp_var(const TypeExpr& t_expr) {
  std::stringstream sstream;
  sstream << "temp_" << temp_var_table.size();
  std::string name = std::string(sstream.str());
  Var var = Var::create_id(name, t_expr);
  temp_var_table[name] = var;
  return var;
}

const std::vector<Var> SymTable::get_variable_list() const {
  std::vector<Var> vars;
  for (auto iter = var_table_.cbegin(); iter != var_table_.cend(); ++iter) {
    vars.push_back(iter->second);
  }
  return vars;
}
