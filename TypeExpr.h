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
  unsigned int size;   // 元素个数
  Type type;           // 元素类型
  unsigned int width;  // 元素宽度，单位为字节
  TypeExpr_ *next;     // 包含的下一个表达式
  TypeExpr_(unsigned int size, Type type, unsigned int width, TypeExpr_ *next);
  virtual ~TypeExpr_();
};
typedef TypeExpr_ TypeExpr;
static TypeExpr base_type(Type type);
static TypeExpr array_type(unsigned int size, TypeExpr *next);
