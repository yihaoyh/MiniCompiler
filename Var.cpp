#include "Var.h"
#include "Type.h"
Var Var::default_ = Var();

/*
 * ����һ��number�����������ڽ�֧������
 */
Var Var::create_number(std::string value)
{
	return Var(Tag::LT_NUMBER, "", value, Type::INT);
}

Var Var::create_id(std::string name, Type type)
{
	return Var(Tag::IDENTIFIER, name, "", type);
}

Var::Var()
{
	tag = Tag::UNKNOWN;
	type = Type::UNKNOWN;
}

Var::Var(Tag t, std::string name, std::string value, Type type) :tag{ t }, name{ name }, value_string{value},type{ type }
{
}

Var& Var::operator=(const Var& ref)
{
	tag = ref.tag;
	type = ref.type;
	name = ref.name;
	value_string = ref.value_string;
	return *this;
}
