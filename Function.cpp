﻿#include "Function.h"

Function::Function(const std::string& fun_name, bool is_declaration,
                   const TypeExpr& t_expr, const std::vector<Var>& params)
    : name{fun_name},
      is_declaration{is_declaration},
      return_type{t_expr},
      params_{params} {
  for (auto iter = params.begin(); iter != params.end(); ++iter) {
    sym_table_.put_variable(*iter);
  }
}

Function::Function() : name{""}, is_declaration{"false"} {
}

/*
 返回指令的标号
*/
instr_number Function::add_instruction(InterInstruction* inst) {
  inst->index = inst_vector_.size();
  inst_vector_.push_back(*inst);
  return inst->index;
}

InterInstruction& Function::get_instruction(unsigned int index) {
  return inst_vector_[index];
}

std::vector<InterInstruction>& Function::get_instructions() {
  return inst_vector_;
}

const std::vector<InterInstruction>& Function::get_const_instructions() const {
  return inst_vector_;
}

const std::vector<Var> Function::get_params() const { return params_; }

instr_number Function::get_next_instruction() { return inst_vector_.size(); }

void Function::put_variable(const Var& var) { sym_table_.put_variable(var); }

const Var Function::get_variable(const std::string& name) const {
  return sym_table_.get_const_variable(name);
}

const std::vector<Var> Function::get_variable_list() const {
  return sym_table_.get_variable_list();
}

Var Function::gen_temp_var(const TypeExpr t_expr) {
  Var var = sym_table_.gen_temp_var(t_expr);
  put_variable(var);
  return var;
}

Var Function::gen_temp_pointer(const TypeExpr t_expr) {
  Var var = sym_table_.gen_temp_var(t_expr);
  var.is_pointer = true;
  put_variable(var);
  return var;
}

std::string Function::gen_label() {
  std::stringstream sstream;
  sstream << ".L_" << name << label_index_++;
  return sstream.str();
}