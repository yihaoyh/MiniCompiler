/*
* �﷨������
*/
#pragma once
#include"Lexer.h"
#include"Var.h"
#include"SymTable.h"
#include"Function.h"
#include"BoolExpr.h"
#include"Statement.h"
class Parser
{
public:
	Parser();
	void begin_parse(const std::string& file_name);
	void print_instructions();
private:
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
	Type type();

	/*
		��������������������
		<def>->ID <idtail>
	*/
	void def(const Type& type);

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
	void defdata(const Type& type);

	/*
		<paradata>->MUL ID | ID <paradatatail>
	*/
	void paradata(std::vector<Var>& params);

	/*
		<paradatatail>->LBRACKET NUM RBRACKET | ��
	*/
	void paradatatail();

	/*
		<para>-><type> <paradata> <paralist> | ��
	*/
	void para(std::vector<Var>& params);

	/*
		<block>->LBRACES <subprogram> RBRACES
	*/
	Statement block();

	/*
		block �ɾֲ�����������������ɻ����ս�����
		<subprogram>-><localdef> <subprogram> | <statement> <subprogram> | ��
	*/
	Statement subprogram();

	/*
		<statement>-><assign> SEMICOLON | KW_RETURN <expr> SEMICOLON | ID ( ) SEMICOLON
	*/
	Statement statement();

	/*
		<assign>-><item><assigntail> | <expr>
	*/
	Var assign_expr();

	/*
	    <assign_tail>-> = <assign_expr>
	*/
	void assign_tail(Var var);

	BoolExpr or_expr();

	BoolExpr or_tail(BoolExpr& expr);

	BoolExpr and_expr();

	BoolExpr and_tail(BoolExpr &expr);

	BoolExpr compare_expr();

	BoolExpr compare_tail(Var &lval, BoolExpr& expr);
	/*
		<localdef>-><type> <defdata> <deflist>
	*/
	void localdef();

	/*
		<expr>-><item> <exprtail>
	*/
	Var expr();

	/*
		<item>-><factor> <itemtail>
	*/
	Var item();

	/*
		<itemtail>-><op_high> <factor> <itemtail> | ��
	*/
	Var itemtail(const Var&);

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

	void funtail(const Type& type, std::string, const std::vector<Var>& params);

	Var idtail(const Type&, std::string);

	Var idexpr(std::string);

	void put_variable(Var var);

	instr_index add_instruction(InterInstruction instrunction);
	
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

	Operator op_compare();

	// ������ʽ
	Var alo_expr();

	Var alo_tail(const Var& var);

	//BoolExpr bool_expr();

	bool match(const Tag& tag);

	// ��������
	void para_transit(); 

	Function get_function(std::string);

	void put_function(Function);

	std::vector<Function> get_functions();

	std::vector<instr_index> merge(std::vector<instr_index>list1, std::vector<instr_index>list2);

	Token token_look_;
	Lexer lexer_;
	//SymTable sym_table_;
	Function* current_function_;
	std::vector<Token> token_stack_;
	std::vector<Function> functions_;
	int index;
	std::map<std::string, Function> function_table_; // ������
};


