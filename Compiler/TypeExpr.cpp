#include "TypeExpr.h"
TypeExpr_::TypeExpr_() {}
TypeExpr_::TypeExpr_(unsigned int size, Type type, unsigned int width,
                     TypeExpr_* next) {
  this->size = size;
  this->type = type;
  this->width = width;
  this->next = next;
}

TypeExpr_::TypeExpr_(const TypeExpr_& lhs) {
  size = lhs.size;
  type = lhs.type;
  width = lhs.width;
  if (lhs.next != nullptr) next = new TypeExpr_(*lhs.next);
}

TypeExpr_::TypeExpr_(TypeExpr_&& rhs) noexcept {
  std::swap(size, rhs.size);
  std::swap(type, rhs.type);
  std::swap(width, rhs.width);
  std::swap(next, rhs.next);
}

TypeExpr_& TypeExpr_::operator=(const TypeExpr_& ref) {
  size = ref.size;
  type = ref.type;
  width = ref.width;
  if (ref.next != nullptr) next = new TypeExpr_(*ref.next);
  return *this;
}

TypeExpr_& TypeExpr_::operator=(TypeExpr_&& ref) noexcept {
  std::swap(size, ref.size);
  std::swap(type, ref.type);
  std::swap(width, ref.width);
  std::swap(next, ref.next);
  return *this;
}

TypeExpr_::~TypeExpr_() {
  if (next != nullptr) delete next;
}

TypeExpr base_type(Type type) {
  return TypeExpr_{0, type, get_type_size(type), nullptr};
}

TypeExpr array_type(unsigned int size, TypeExpr* next) {
  return TypeExpr_{size, next->type, next->width * next->size, next};
}
// static const TypeExpr &UNKNOWN_TYPE_EXPR
