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
	idtail(tag, name);
}

void Parser::idtail(Tag tag, std::string name)
{
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
		varrdef();
		move_token();
		deflist();
	}
}

void Parser::varrdef()
{
	init();
}

void Parser::init()
{
	if (FIRST(ASSIGN))
	{
		move_token();
		expr();
	}
	else if (FIRST(END_OF_FILE))
	{
		return;
	}
	else {
		recovery();
	}
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

void Parser::expr()
{
	item();
	move_token();
	exprtail();
}

/*
	<exprtail>-><op_low> <item> <exprtail> | SEMICOLON
*/
void Parser::exprtail()
{
	if (FIRST(SEMICOLON))
	{
		return;
	}
	else 
	{
		op_low();
		move_token();
		item();
		move_token();
		exprtail();
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

void Parser::item()
{
	factor();
	itemtail();
}

// <itemtail>-><op_high> <factor> <itemtail>
void Parser::itemtail()
{
	if (FIRST(MULTIPLY)OR(DIVIDE)) 
	{
		op_high();
		move_token();
		factor();
		move_token();
		itemtail();
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

void Parser::factor()
{
	val();
}

void Parser::val()
{
	elem();
}

void Parser::elem()
{
	if (FIRST(IDENTIFIER))
	{
		return;
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
		literal();
	}

}

void Parser::literal()
{
	if (FIRST(LT_NUMBER)OR(LT_CHAR)OR(LT_STRING))
	{
		return;
	}
	else 
	{
		recovery();
	}
}

void Parser::recovery()
{
}

void Parser::move_token()
{
	token_look_ = lexer_.tokenize();
}
