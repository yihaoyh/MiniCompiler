#pragma once
#include<string>
#include<sstream>
enum Tag			// 词法标签
{
	UNKNOWN,        // 未知标签
	ERROR,			// 错误
	END_OF_FILE,    // 文件结束
	COMMENT,		// 注释
	KW_INT,			// keyword int
	KW_CHAR,		// keyword char
	KW_STRING,		// keyword string
	KW_IF,			// keyword if
	KW_ELSE,		// keyword else
	KW_RETURN,		// keyword return
	KW_VOID,        // keyword void
	LT_NUMBER,		// 数字字面量
	LT_CHAR,		// 字符字面量
	LT_STRING,		// 字符串字面量
	LPAREN,			// (
	RPAREN,			// )
	LBRACES,		// {
	RBRACES,		// }
	LBRACKET,		// [
	RBRACKET,		// ]
	ADD,			// +
	SUB,			// -
	MULTIPLY,		// *
	DIVIDE,			// /
	COMMA,			// ,
	ASSIGN,			// =
	EQUAL,			// ==
	SEMICOLON,		// ;
	IDENTIFIER      // identifier
};

class Token 
{
private:
	Tag tag_;
	std::string string_;
	std::string get_tag_string()
	{
		switch (tag_)
		{
		case UNKNOWN:
			return "unknwon";
		case ERROR:
			return "error";
		case END_OF_FILE:
			return "end of file";
		case COMMENT:
			return "comment";
		case KW_INT:
			return "keyword int";
		case KW_CHAR:
			return "keyword char";
		case KW_STRING:
			return "keyword string";
		case KW_IF:
			return "keyword if";
		case KW_ELSE:
			return "keyword else";
		case KW_RETURN:
			return "keyword return";
		case KW_VOID:
			return "keyword void";
		case LT_NUMBER:
			return "literal number";
		case LT_CHAR:
			return "literal char";
		case LT_STRING:
			return "literal string";
		case LPAREN:
			return "(";
		case RPAREN:
			return ")";
		case LBRACES:
			return "{";
		case RBRACES:
			return "}";
		case LBRACKET:
			return "[";
		case RBRACKET:
			return "]";
		case ADD:
			return "+";
		case SUB:
			return "-";
		case MULTIPLY:
			return "*";
		case DIVIDE:
			return "/";
		case COMMA:
			return ",";
		case SEMICOLON:
			return ";";
		case ASSIGN:
			return "=";
		case EQUAL:
			return "==";
		case IDENTIFIER:
			return "identifier";
		default:
			return "unknown";
		}
	}
public:
	Token(Tag tag, std::string str) :tag_{ tag }, string_{str}
	{
	}
	Tag tag()
	{
		return tag_;
	}
	virtual ~Token() {}
	virtual std::string to_string()
	{
		std::ostringstream ostr;
		ostr << "Tag:" << get_tag_string() << " string:" << string_;
		return std::string(ostr.str());
	}


};

class Lexer
{
public:
	Lexer();
	void scan(const char* file_name);
	char get_char();
	void reset();

private:
	Token* p_current_token_;
	std::string file_string_;
	unsigned int scan_pos_;
	std::string token_string;

	void parse_number(char);
	void parse_word(char);
	void parse_character(char);
	void parse_comment(char ch);
};