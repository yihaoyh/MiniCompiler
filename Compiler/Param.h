#pragma once
#include "Var.h"
class Param {
 public:
  Var var;
  bool isEscape;  // 是否逃逸

  Param(Var var, bool isEscape) : var{var}, isEscape{isEscape} {}

  Param& operator=(const Param& ref);
};
