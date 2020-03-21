#include"Common.h"
#include<map>
#include<string>
#include"Lexer.h"
#include"Utils.h"
std::map<std::string, Tag> keyword_table;

void init_keyword_table() 
{
	keyword_table["int"] = KW_INT;
	keyword_table["char"] = KW_CHAR;
	keyword_table["string"] = KW_STRING;
	keyword_table["if"] = KW_IF;
	keyword_table["else"] = KW_ELSE;
	keyword_table["while"] = KW_ELSE;
	keyword_table["return"] = KW_RETURN;
	keyword_table["void"] = KW_VOID;
}

Tag get_keyword_tag(std::string key)
{
	if (keyword_table.find(key)!=keyword_table.end())
	{
		return keyword_table[key];
	}
	return UNKNOWN;
}

void print_error(const char* message)
{
	error(message);
}
