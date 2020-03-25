#include "TypeExpr.h"
TypeExpr_::TypeExpr_(unsigned int size, Type type, unsigned int width,
                     TypeExpr_* next) {
  this->size = size;
  this->type = type;
  this->width = width;
  this->next = next;
}

TypeExpr_::~TypeExpr_() {
  if (next != nullptr) delete next;
}

TypeExpr base_type(Type type) {
  return TypeExpr_{0, type, get_type_size(type),
                   nullptr};
}

TypeExpr array_type(unsigned int size, TypeExpr* next) {
  return TypeExpr_{size, next->type, next->width * next->size, next};
}
