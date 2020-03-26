#include "Var.h"

#include "Type.h"
 const Var Var::default_ = Var();

/*
 * 创建一个number字面量，现在仅支持整型
 */
Var Var::create_number(std::string value) {
  return Var(Tag::LT_NUMBER, "", value, base_type(Type::INT));
}

Var Var::create_id(std::string name, const TypeExpr& t_expr) {
  return Var(Tag::IDENTIFIER, name, "", t_expr);
}

Var::Var() {
  tag = Tag::UNKNOWN;
  type_expr = UNKNOWN_TYPE_EXPR;
}

Var::Var(Tag t, std::string name, std::string value, const TypeExpr& t_expr)
    : tag{t}, name{name}, value_string{value}, type_expr{t_expr} {}

Var& Var::operator=(const Var& ref) {
  tag = ref.tag;
  type_expr = ref.type_expr;
  name = ref.name;
  value_string = ref.value_string;
  return *this;
}
