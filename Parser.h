/*
* �﷨������
*/
#pragma once
#include"Lexer.h"
class Parser
{
public:
	Parser();
	void begin_parse();
private:
	Token token_look_;
	Lexer lexer_;

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

	void idtail(Tag, std::string);
	/*
		<varrdef>-><init>    
	*/
	void varrdef();
	
	/*
		<init>-> ASSIGN <expr> | ��
	*/
	void init();

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
	void expr();
	/*
		<exprtail>-><op_low> <item> <exprtail> | SEMICOLON
	*/
	void exprtail();
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
	void item();
	/*
		<itemtail>-><op_high> <factor> <itemtail> | ��
	*/
	void itemtail();
	/*
		<op_high>->* | /
	*/
	void op_high();
	/*
		<factor>->val
	*/
	void factor();
	/*
		<val>-><elem>
	*/
	void val();
	/*
		<elem>->ID | <LPAREN> <expr> <RPAREN> | <literal>
	*/
	void elem();
	/*
		<literal>->NUM | CHAR |STRING
	*/
	void literal();

	/*
		�޸�����
	*/
	void recovery();

	/*
		�ƶ�����һ��token
	*/
	void move_token();

};

