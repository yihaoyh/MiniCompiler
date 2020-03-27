#pragma once
#include "Type.h"
/*
    Ӧ������������ͱ��ʽ
    ����: int a[2][3] ��Ӧ�ı��ʽΪ{2,INT,12,{3,INT,4, null_ptr}}
    ����a[i][j]ʱ������������ָ�
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
  unsigned int size = 0;      // Ԫ�ظ���
  Type type = Type::UNKNOWN;  // Ԫ������
  unsigned int width = 0;     // Ԫ�ؿ�ȣ���λΪ�ֽ�
  TypeExpr_ *next = nullptr;  // ��������һ�����ʽ
};
typedef TypeExpr_ TypeExpr;
TypeExpr base_type(Type type);
TypeExpr array_type(unsigned int size, TypeExpr *next);

const TypeExpr UNKNOWN_TYPE_EXPR = TypeExpr{0, Type::UNKNOWN, 0, nullptr};
;
