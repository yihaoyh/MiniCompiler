#include "Var.h"

Var::Var()
{
	tag = UNKNOWN;
}

Var::Var(Tag t, std::string str) :tag{ t }, name{str}
{
}
