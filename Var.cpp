#include "Var.h"

Var Var::default_ = Var();

Var::Var()
{
	tag = UNKNOWN;
	type = VarType::VAR;
}

Var::Var(Tag t, std::string str, VarType type) :tag{ t }, name{ str }, type{type}
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
