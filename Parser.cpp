#include "Parser.h"
#include<string>
#include<iostream>
#define FIRST(TAG) token_look_.tag() == TAG
#define OR(TAG) ||token_look_.tag() == TAG
#define TYPE_FIRST FIRST(KW_INT)OR(KW_CHAR)OR(KW_STRING)OR(KW_VOID)
//表达式
#define EXPR_FIRST FIRST(LPAREN)OR(LT_NUMBER)OR(LT_CHAR)OR(LT_STRING)OR(IDENTIFIER)OR(SUB)OR(MULTIPLY)
#define STATEMENT_FIRST EXPR_FIRST OR(KW_IF)OR(KW_RETURN)
Parser::Parser()
{
	current_function_ = nullptr;
	index = -1;
}
void Parser::begin_parse()
{
	lexer_.load_file("parser_test.txt");
	move_token();
	program();
}
void Parser::print_instructions()
{
	std::vector<Function> functions = sym_table_.get_functions();
	for (unsigned int i = 0; i < functions.size(); i++)
	{
		Function fun = functions[i];
		std::cout << "function " << fun.name << " begin" << std::endl;
		std::vector<InterInstruction> insts = fun.get_instructions();
		for (unsigned int j = 0; j < insts.size(); j++)
		{
			std::cout << "    " << insts[j].to_string() << std::endl;
		}
		std::cout << "function " << fun.name << " end" << std::endl;
	}
}
void Parser::program()
{
	//move_token();
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
	if (TYPE_FIRST)
	{
		return token_look_.tag();
	}
	else
	{
		recovery();
		return UNKNOWN;
	}
}
// <def>->ID <idtail> 代码形式如 " a = a + b"
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
}

Var Parser::varrdef(Var result)
{
	return init(result);
}

// <init>-> ASSIGN <expr> | ε  代码"= a * b"
Var Parser::init(Var result)
{
	Var var;
	if (FIRST(ASSIGN))
	{
		move_token();
		var = expr(&result);
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
	//move_token();
}

// <defdata>->ID <varrdef> 对应代码"a = 10 * b"
void Parser::defdata()
{
	if (FIRST(IDENTIFIER))
	{
		std::string name = token_look_.get_name();
		move_token();
		Var result;
		result.tag = IDENTIFIER;
		result.name = name;
		put_variable(result);
		Var arg1 = varrdef(result);
		//put_variable(arg1);

		//Operator op = Operator::OP_ASSIGN;
		//add_instruction(InterInstruction(result, op, arg1, Var::default_));
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
	if (FIRST(LBRACES))
	{
		move_token();
		subprogram();
		move_token();
		if (FIRST(RBRACES))
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
	// <subprogram>-><localdef> <subprogram> | <statement> <subprogram> | ε
	if (FIRST(END_OF_FILE))
	{
		return;
	}
	else if (TYPE_FIRST)
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
		Var result = expr(nullptr);
		//move_token();
		if (FIRST(SEMICOLON))
		{
			add_instruction(InterInstruction(Var::default_, Operator::OP_RETURN, result, Var::default_));
			return;
		}
		else
		{
			recovery();
		}
	}
	//else if (FIRST(IDENTIFIER))
	//{
	//	std::cout << "wrong" << std::endl;
	//	std::string name = token_look_.get_name();
	//	move_token();
	//	idexpr(name);
	//}
	else
	{
		Var var = expr(nullptr);
		put_variable(var);
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
	move_token();
	defdata();
	move_token();
	deflist();
	if (FIRST(SEMICOLON))
	{
		move_token();
	}
	else 
	{
		recovery();
	}
}

/*
	<expr>-><item> <exprtail> 对应代码 "a + b","a = 10"
	此处因为expr和assign混用，导致一系列问题，需要重构
*/
Var Parser::expr(Var *presult)
{
	Var var = item();
	//move_token();
	return exprtail(presult, var);
}

/*
	<exprtail>-><op_low> <item> <exprtail> | SEMICOLON 对应代码 "c;"," + a;", " + a + b;"
*/
Var Parser::exprtail(Var* result, Var arg1)
{
	if (FIRST(ASSIGN)) {
		move_token();
		Var var = expr(result);
		add_instruction(InterInstruction(*result, Operator::OP_ASSIGN, var, Var::default_));
	}
	else {
		move_token();
		if (FIRST(ADD)OR(SUB))
		{
			Operator op = op_low();
			move_token();
			Var arg2 = item();

			move_token();
			if (FIRST(ADD)OR(SUB))
			{
				Var temp = sym_table_.gen_temp_var();
				add_instruction(InterInstruction(temp, op, arg1, arg2));
				back_token();
				return exprtail(result, temp);
			}
			else
			{
				if (result == nullptr)
				{
					result = &Var();
				}
				add_instruction(InterInstruction(*result, op, arg1, arg2));
				back_token();
			}
		}
		else
		{
			back_token();
		}
	}
	
	return arg1;
}

void Parser::operator_()
{
}

void Parser::operand()
{
}

Operator Parser::op_low()
{
	if (FIRST(ADD)OR(SUB))
	{
		switch (token_look_.tag())
		{
		case ADD:
			return Operator::OP_ADD;
		case SUB:
			return Operator::OP_SUB;
		}
	}
	else
	{
		recovery();
		return Operator::OP_INVALID;
	}
}
// 对应代码 10 * a
Var Parser::item()
{
	Var var = factor();
	//move_token();
	var = itemtail(var);
	std::cout << "item " << token_look_.get_name() << std::endl;
	return var;
}

// <itemtail>-><op_high> <factor> <itemtail>  对应代码 * b * c 
Var Parser::itemtail(Var var)
{
	if (FIRST(MULTIPLY)OR(DIVIDE))
	{
		Var arg1 = var;
		Operator op = op_high();
		move_token();
		Var arg2 = factor();
		Var result = sym_table_.gen_temp_var();
		add_instruction(InterInstruction(result, op, arg1, arg2));
		move_token();
		return itemtail(result);
	}
	else
	{
		return var;
	}
}

Operator Parser::op_high()
{
	if (FIRST(MULTIPLY)OR(DIVIDE))
	{
		switch (token_look_.tag())
		{
		case MULTIPLY:
			return Operator::OP_MUL;
		case DIVIDE:
			return Operator::OP_DIV;
		}
	}
	else
	{
		recovery();
		return Operator::OP_INVALID;
	}
}

/* <factor> -> val factor()会多读一个token */
Var Parser::factor()
{
	return val();
}

/* <val> -> elem */
Var Parser::val()
{
	return elem();
}

/* <elem>-> ID | (expr) | literal   elem() 会多读一个token*/ 
Var Parser::elem()
{
	Var var;
	if (FIRST(IDENTIFIER))
	{
		// 从符号表中读取变量
		std::string name = token_look_.get_name();
		move_token();
		var = idexpr(name);

	}
	else if (FIRST(LPAREN))
	{
		move_token();
		expr(nullptr);
		move_token();
		if (!(FIRST(RPAREN))) {
			recovery();
		}
	}
	else
	{
		var = literal();
		//move_token();
	}
	return var;
}

Var Parser::literal()
{
	if (FIRST(LT_NUMBER)OR(LT_CHAR)OR(LT_STRING))
	{
		Var var = Var(token_look_.tag(), "", VarType::VAR);
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
	std::cout << "recovery " << std::endl;
}

void Parser::move_token()
{
	if(index == token_stack_.size() - 1)
	{
		do
		{
			token_look_ = lexer_.tokenize();
		} while (token_look_.tag() == COMMENT);
		token_stack_.push_back(token_look_);
		index++;
	}
	else {
		token_look_ = token_stack_[++index];
	}

}

// <idtail>-><varrdef> <deflist> | LPAREN <para> RPAREN <funtail>
Var Parser::idtail(Tag, std::string name)
{
	Var var;
	if (FIRST(LPAREN))
	{
		move_token();
		para();
		//move_token();
		if (!(FIRST(RPAREN)))
		{
			recovery();
		}
		move_token();
		funtail(name);
	}
	else {
		Var result = Var(IDENTIFIER, name, VarType::VAR);
		var = varrdef(result);
		move_token();
		deflist();
		var.name = name;
		put_variable(var);
		//sym_table_.put_variable(var);
	}
	return var;
}

void Parser::funtail(std::string fun_name)
{
	if (FIRST(SEMICOLON))
	{
		std::cout << "function declaration occur name:" << fun_name << std::endl;
		Function func = Function(fun_name, true);
		sym_table_.put_function(func);
	}
	else if (FIRST(LBRACES)) {
		std::cout << "function definition occur name:" << fun_name << std::endl;
		Function func = Function(fun_name, false);
		current_function_ = &func;
		block();
		sym_table_.put_function(func);
		current_function_ = nullptr;
	}
	move_token();
}

Var Parser::idexpr(std::string name)
{
	//move_token();
	Var var;
	if (FIRST(LPAREN))
	{
		move_token();
		if (FIRST(RPAREN) == false)
		{
			recovery();
		}
		// 函数调用
		Function fun = sym_table_.get_function(name);
		if (fun.name != std::string(""))
		{
			// 待填充
			std::cout << "function call occur : " << name << "()" << std::endl;
			Var arg1 = Var(IDENTIFIER, name, VarType::VAR);
			var = sym_table_.gen_temp_var();
			InterInstruction inst = InterInstruction(var, Operator::OP_CALL, arg1, Var::default_);
			//sym_table_.add_instruction(inst);
			add_instruction(inst);
		}
		//move_token();
	}
	else
	{
		var = sym_table_.get_variable(name);
		//back_token();

	}
	return var;
}

void Parser::put_variable(Var var)
{
	sym_table_.put_variable(var);
	//if (current_function_ != nullptr)
	//{
	//	current_function_->put_variable(var);
	//}
}

void Parser::add_instruction(InterInstruction instrunction)
{
	sym_table_.add_instruction(instrunction);
	if (current_function_ != nullptr) {
		current_function_->add_instruction(instrunction);
	}
}

void Parser::back_token()
{
	if (index > 0)
	{
		token_look_ = token_stack_[--index];
	}

	
	//token_stack_.pop_back();
}
