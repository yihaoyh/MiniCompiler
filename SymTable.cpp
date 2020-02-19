#include "SymTable.h"

void SymTable::put_sym(Var var)
{
	var_table_[var.name] = var;
}
