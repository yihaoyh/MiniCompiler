#include "Type.h"

Type tag_to_type(Tag tag) {
  Type type = Type::UNKNOWN;
  switch (tag) {
    case Tag::KW_INT:
      type = Type::INT;
      break;
    case Tag::KW_CHAR:
      type = Type::CHAR;
      break;
    case Tag::KW_STRING:
      type = Type::STRING;
      break;
    case Tag::LT_NUMBER:
      type = Type::INT;
      break;
    case Tag::LT_CHAR:
      type = Type::CHAR;
      break;
    case Tag::LT_STRING:
      type = Type::STRING;
      break;
    case Tag::KW_VOID:
      type = Type::VOID;
      break;
    default:
      Type::UNKNOWN;
      break;
  }
  return type;
}

/*
    获取类型的长度，单位为字节
*/
unsigned int get_type_length(const Type& type) {
  switch (type) {
    case Type::INT:
      return 8;
  }
  return 0;
}