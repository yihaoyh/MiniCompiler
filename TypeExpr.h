#pragma once
#include "Type.h"
/*
    应用于数组的类型表达式
    例如: int a[2][3] 对应的表达式为{2,INT,12,{3,INT,4, null_ptr}}
    访问a[i][j]时，会生成如下指令：
    t0 = i * 12;
    t1 = j * 4;
    t2 = t0 + t1;
    t3 = a offset t2
*/
struct TypeExpr_ {
  TypeExpr_();
  TypeExpr_(unsigned int size, Type type, unsigned int width, TypeExpr_ *next);
  TypeExpr_(const TypeExpr_ &lhs);
  TypeExpr_(TypeExpr_ &&rhs) noexcept;
  TypeExpr_ &operator=(const TypeExpr_ &lhs);
  TypeExpr_ &operator=(TypeExpr_ &&rhs) noexcept;
  virtual ~TypeExpr_();
  unsigned int size = 0;      // 元素个数
  Type type = Type::UNKNOWN;  // 元素类型
  unsigned int width = 0;     // 元素宽度，单位为字节
  TypeExpr_ *next = nullptr;  // 包含的下一个表达式
};
typedef TypeExpr_ TypeExpr;
TypeExpr base_type(Type type);
TypeExpr array_type(unsigned int size, TypeExpr *next);

const TypeExpr UNKNOWN_TYPE_EXPR = TypeExpr{0, Type::UNKNOWN, 0, nullptr};
;
