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
  unsigned int size;   // Ԫ�ظ���
  Type type;           // Ԫ������
  unsigned int width;  // Ԫ�ؿ�ȣ���λΪ�ֽ�
  TypeExpr_ *next;     // ��������һ�����ʽ
  TypeExpr_(unsigned int size, Type type, unsigned int width, TypeExpr_ *next);
  virtual ~TypeExpr_();
};
typedef TypeExpr_ TypeExpr;
static TypeExpr base_type(Type type);
static TypeExpr array_type(unsigned int size, TypeExpr *next);
