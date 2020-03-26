#pragma once
#include <string>

#include "Lexer.h"
#include "Type.h"
#include "TypeExpr.h"
enum class VarType {
  VAR,  // 变量
  TEMP  // 临时变量
};

/*
        变量类
*/
class Var {
 public:
  static Var create_number(std::string value);
  static Var create_id(std::string name,const TypeExpr& t_expr);
  Var();
  Var(Tag, std::string, std::string,const TypeExpr& t_expr);
  Var &operator=(const Var &ref);

  TypeExpr type_expr = UNKNOWN_TYPE_EXPR;                 // 变量类型
  Tag tag;                   // 变量标签
  std::string name;          // 变量名称
  std::string value_string;  // 值的字符串形式
  const static Var default_;
};
