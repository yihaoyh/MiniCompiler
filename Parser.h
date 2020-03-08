/*
* �﷨������
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
		������������ <program>-><segment> <program> | ��
	*/
	void program();

	/* 
	   ����Ƭ�Σ������������������壬�������������壬��������
	   <segment>-><type><def>
	*/ 
	void segment();

	/*
		���� <type>->KW_INT | KW_CHAR | KW_STRING | KW_VOID
	*/
	Tag type();

	/*
		��������������������
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
		<init>-> ASSIGN <expr> | ��
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
		<paradatatail>->LBRACKET NUM RBRACKET | ��
	*/
	void paradatatail();

	/*
		<para>-><type> <paradata> <paralist> | ��
	*/
	void para();

	/*
		<block>->LBRACES <subprogram> RBRACES
	*/
	void block();

	/*
		block �ɾֲ�����������������ɻ����ս�����
		<subprogram>-><localdef> <subprogram> | <statement> <subprogram> | ��
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
		<itemtail>-><op_high> <factor> <itemtail> | ��
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
		�޸�����
	*/
	void recovery();

	/*
		��ǰ�ƶ�����һ��token
	*/
	void move_token();

	void funtail(std::string);

	Var idtail(Tag, std::string);

	Var idexpr(std::string);

	void put_variable(Var var);

	void add_instruction(InterInstruction instrunction);
	
	/*
		�����ƶ�һ��token
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


