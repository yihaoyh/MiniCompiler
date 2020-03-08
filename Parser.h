/*
* 语法分析器
*/
#pragma once
#include"Lexer.h"
#include"Var.h"
#include"SymTable.h"
class Parser
{
public:
	Parser();
	void begin_parse();
	void print_instructions();
private:
	Token token_look_;
	Lexer lexer_;
	SymTable sym_table_;
	Function* current_function_;
	std::vector<Token> token_stack_;
	int index;

	/* 
		分析整个程序 <program>-><segment> <program> | ε
	*/
	void program();

	/* 
	   程序片段，包括变量声明、定义，函数声明、定义，函数调用
	   <segment>-><type><def>
	*/ 
	void segment();

	/*
		类型 <type>->KW_INT | KW_CHAR | KW_STRING | KW_VOID
	*/
	Tag type();

	/*
		变量、函数的声明或定义
		<def>->ID <idtail>
	*/
	void def(Tag);

	/*
		<idtail>-><varrdef> <deflist> | LPAREN <para> RPAREN <funtail>
	*/
	
	/*
		<varrdef>-><init>    
	*/
	Var varrdef(Var result);
	
	/*
		<init>-> ASSIGN <expr> | ε
	*/
	Var init(Var result);

	/*
		<deflist>->COMMA <defdata> <deflist> | SEMICOLON
	*/
	void deflist();

	/*
		<defdata>->ID <varrdef>
	*/
	void defdata();

	/*
		<paradata>->MUL ID | ID <paradatatail>
	*/
	void paradata();

	/*
		<paradatatail>->LBRACKET NUM RBRACKET | ε
	*/
	void paradatatail();

	/*
		<para>-><type> <paradata> <paralist> | ε
	*/
	void para();

	/*
		<block>->LBRACES <subprogram> RBRACES
	*/
	void block();

	/*
		block 由局部变量定义或者语句组成或者终结符组成
		<subprogram>-><localdef> <subprogram> | <statement> <subprogram> | ε
	*/
	void subprogram();

	/*
		<statement>-><assign> SEMICOLON | KW_RETURN <expr> SEMICOLON | ID ( ) SEMICOLON
	*/
	void statement();

	/*
		<assign>-><item><assigntail> | <expr>
	*/
	Var assign_expr();

	/*
	    <assign_tail>->=<assign_tail>
	*/
	Var assign_tail(Var var);

	/*
		<localdef>-><type> <defdata> <deflist>
	*/
	void localdef();

	/*
		<expr>-><item> <exprtail>
	*/
	Var expr(Var *presult);

	/*
		<exprtail>-><op_low> <item> <exprtail> | SEMICOLON
	*/
	Var exprtail(Var *result, Var arg1);

	/*
		<item>-><factor> <itemtail>
	*/
	Var item();

	/*
		<itemtail>-><op_high> <factor> <itemtail> | ε
	*/
	Var itemtail(Var);

	/*
		<op_high>->* | /
	*/
	Operator op_high();

	/*
		<factor>->val
	*/
	Var factor();

	/*
		<val>-><elem>
	*/
	Var val();

	/*
		<elem>->ID | <LPAREN> <expr> <RPAREN> | <literal>
	*/
	Var elem();

	/*
		<literal>->NUM | CHAR |STRING
	*/
	Var literal();

	/*
		修复错误
	*/
	void recovery();

	/*
		向前移动解析一个token
	*/
	void move_token();

	void funtail(std::string);

	Var idtail(Tag, std::string);

	Var idexpr(std::string);

	void put_variable(Var var);

	void add_instruction(InterInstruction instrunction);
	
	/*
		往后移动一个token
	*/
	void back_token();

	/*
	<operator>
*/
	void operator_();

	/*
		<operand>
	*/
	void operand();

	/*
		<op_low>->+ | -
	*/
	Operator op_low();

	Var alo_expr();

	Var alo_tail(Var var);

	bool match(Tag tag);
};


