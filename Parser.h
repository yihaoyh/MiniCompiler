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
private:
	Token token_look_;
	Lexer lexer_;
	SymTable sym_table_;

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
	Var varrdef();
	
	/*
		<init>-> ASSIGN <expr> | ��
	*/
	Var init();

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
		<statement>-><expr> SEMICOLON | KW_RETURN <expr> SEMICOLON
	*/
	void statement();
	/*
		<localdef>-><type> <defdata> <deflist>
	*/
	void localdef();
	/*
		<expr>-><item> <exprtail>
	*/
	Var expr();
	/*
		<exprtail>-><op_low> <item> <exprtail> | SEMICOLON
	*/
	Var exprtail(Var);
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
	void op_low();
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
	void op_high();
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
		�ƶ�����һ��token
	*/
	void move_token();

	void funtail();

	Var idtail(Tag, std::string);
};


