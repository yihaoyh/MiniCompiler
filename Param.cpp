#include "Param.h"

Param& Param::operator=(const Param& ref) {
  var = ref.var;
  isEscape = ref.isEscape;
  return *this;
}