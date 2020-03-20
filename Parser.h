/*
* 语法分析器
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
	Type type();

	/*
		变量、函数的声明或定义
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
	void defdata(const Type& type);

	/*
		<paradata>->MUL ID | ID <paradatatail>
	*/
	void paradata(std::vector<Var>& params);

	/*
		<paradatatail>->LBRACKET NUM RBRACKET | ε
	*/
	void paradatatail();

	/*
		<para>-><type> <paradata> <paralist> | ε
	*/
	void para(std::vector<Var>& params);

	/*
		<block>->LBRACES <subprogram> RBRACES
	*/
	Statement block();

	/*
		block 由局部变量定义或者语句组成或者终结符组成
		<subprogram>-><localdef> <subprogram> | <statement> <subprogram> | ε
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
		<itemtail>-><op_high> <factor> <itemtail> | ε
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
		修复错误
	*/
	void recovery();

	/*
		向前移动解析一个token
	*/
	void move_token();

	void funtail(const Type& type, std::string, const std::vector<Var>& params);

	Var idtail(const Type&, std::string);

	Var idexpr(std::string);

	void put_variable(Var var);

	instr_index add_instruction(InterInstruction instrunction);
	
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

	Operator op_compare();

	// 运算表达式
	Var alo_expr();

	Var alo_tail(const Var& var);

	//BoolExpr bool_expr();

	bool match(const Tag& tag);

	// 参数传递
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
	std::map<std::string, Function> function_table_; // 函数表
};


