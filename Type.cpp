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
