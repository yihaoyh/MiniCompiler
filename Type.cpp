#include"Type.h"

Type tag_to_type(Tag tag)
{
	Type type = Type::UNKNOWN;
	switch (tag)
	{
	case KW_INT:
		type = Type::INT;
		break;
	case KW_CHAR:
		type = Type::CHAR;
		break;
	case KW_STRING:
		type = Type::STRING;
		break;
	case LT_NUMBER:
		type = Type::INT;
		break;
	case LT_CHAR:
		type = Type::CHAR;
		break;
	case LT_STRING:
		type = Type::STRING;
		break;
	case KW_VOID:
		type = Type::VOID;
		break;
	default:
		Type::UNKNOWN;
		break;
	}
	return type;
}
