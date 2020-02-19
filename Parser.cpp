#include "Parser.h"
#include<string>
#define FIRST(TAG) token_look_.tag() == TAG
#define OR(TAG) ||token_look_.tag() == TAG
#define TYPE_FIRST FIRST(KW_INT)OR(KW_CHAR)OR(KW_STRING)OR(KW_VOID)
//±Ì¥Ô Ω
#define EXPR_FIRST FIRST(LPAREN)OR(LT_NUMBER)OR(LT_CHAR)OR(LT_STRING)OR(IDENTIFIER)OR(SUB)OR(MULTIPLY)
#define STATEMENT_FIRST EXPR_FIRST OR(SEMICOLON)OR(KW_IF)OR(KW_RETURN)
Parser::Parser()
{
	
}
void Parser::begin_parse()
{
	lexer_.load_file("parser_test.txt");
	token_look_ = lexer_.tokenize();
	program();
}
void Parser::program()
{
	if (FIRST(END_OF_FILE))
	{
		return;
	}
	else 
	{
		segment();
		program();
	}
}

void Parser::segment()
{
	Tag tag = type();
	move_token();
	def(tag);
}

Tag Parser::type()
{
	if(TYPE_FIRST)
	{
		return token_look_.tag();
	}
	else 
	{
		recovery();
		return UNKNOWN;
	}
}
// <def>->ID <idtail>
void Parser::def(Tag tag)
{
	std::string name = "";
	if (FIRST(IDENTIFIER))
	{
		name = token_look_.get_name();
		move_token();
	}
	else 
	{
		recovery();
	}
	Var var = idtail(tag, name);
	var.name = name;
	sym_table_.put_sym(var);
}

// <idtail>-><varrdef> <deflist> | LPAREN <para> RPAREN <funtail>
Var Parser::idtail(Tag tag, std::string name)
{
	Var var;
	if (FIRST(LPAREN))
	{
		move_token();
		para();
		move_token();
		if (!(FIRST(RPAREN)))
		{
			recovery();
		}
	}
	else {
		var = varrdef();
		move_token();
		deflist();
	}
	return var;
}

Var Parser::varrdef()
{
	return init();
}

// <init>-> ASSIGN <expr> | ¶≈
Var Parser::init()
{
	Var var;
	if (FIRST(ASSIGN))
	{
		move_token();
		var = expr();
	}
	else if (FIRST(END_OF_FILE))
	{
		return var;
	}
	else {
		recovery();
	}
	return var;
}

void Parser::deflist()
{
}

// <defdata>->ID <varrdef>
void Parser::defdata()
{
	if (FIRST(IDENTIFIER))
	{
		move_token();
		varrdef();
	}
}

void Parser::paradata()
{
}

void Parser::paradatatail()
{
}

void Parser::para()
{
}

void Parser::block()
{
	if (FIRST(LPAREN))
	{
		subprogram();
		move_token();
		if (FIRST(RPAREN))
		{
			return;
		}
		else
		{
			recovery();
		}
	}
}

void Parser::subprogram()
{
	// <subprogram>-><localdef> <subprogram> | <statement> <subprogram> | ¶≈
	if (FIRST(END_OF_FILE))
	{
		return;
	}
	else if(TYPE_FIRST)
	{
		localdef();
		subprogram();
	}
	else if (STATEMENT_FIRST)
	{
		statement();
		subprogram();
	}
}

//<expr> SEMICOLON | KW_RETURN <expr> SEMICOLON
void Parser::statement()
{
	if (FIRST(KW_RETURN))
	{
		move_token();
		expr();
		move_token();
		if (FIRST(SEMICOLON))
		{
			return;
		}
		else
		{
			recovery();
		}
	}
	else 
	{
		expr();
		move_token();
		if (FIRST(SEMICOLON))
		{
			return;
		}
		else
		{
			recovery();
		}
	}
}
// <localdef>-><type> <defdata> <deflist>
void Parser::localdef()
{
	type();
	defdata();
	deflist();
}

Var Parser::expr()
{
	Var var = item();
	move_token();
	return exprtail(var);
}

/*
	<exprtail>-><op_low> <item> <exprtail> | SEMICOLON
*/
Var Parser::exprtail(Var var)
{
	if (FIRST(SEMICOLON))
	{
		return var;
	}
	else 
	{
		op_low();
		move_token();
		Var var = item();
		move_token();
		return exprtail(var);
	}
}

void Parser::operator_()
{
}

void Parser::operand()
{
}

void Parser::op_low()
{
	if (FIRST(ADD)OR(SUB))
	{
		return;
	}
	else 
	{
		recovery();
	}
}

Var Parser::item()
{
	Var var = factor();
	return itemtail(var);
}

// <itemtail>-><op_high> <factor> <itemtail>
Var Parser::itemtail(Var var)
{
	if (FIRST(MULTIPLY)OR(DIVIDE)) 
	{
		op_high();
		move_token();
		factor();
		move_token();
		return itemtail(var);
	}
	else {
		return var;
	}
}

void Parser::op_high()
{
	if (FIRST(MULTIPLY)OR(DIVIDE))
	{
		return;
	}
	else
	{
		recovery();
	}
}

Var Parser::factor()
{
	return val();
}

Var Parser::val()
{
	return elem();
}

Var Parser::elem()
{
	Var var;
	if (FIRST(IDENTIFIER))
	{
	}
	else if (FIRST(LPAREN))
	{
		move_token();
		expr();
		move_token();
		if (FIRST(RPAREN) == false) {
			recovery();
		}
	}
	else
	{
		var = literal();
	}
	return var;
}

Var Parser::literal()
{
	if (FIRST(LT_NUMBER)OR(LT_CHAR)OR(LT_STRING))
	{
		Var var = Var(token_look_.tag(), "");
		var.value_string = token_look_.get_name();
		return var;
	}
	else 
	{
		recovery();
		return Var();
	}
}

void Parser::recovery()
{
}

void Parser::move_token()
{
	token_look_ = lexer_.tokenize();
}
