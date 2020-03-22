#pragma once
#include<string>
#include<sstream>
enum class Tag	    // 词法标签
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
	KW_WHILE,       // keyword while
	KW_DO,          // keyword do
	KW_FOR,         // keyword for
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
	LESS,			// <
	LESS_EQUAL,     // <=
	GREATER,        // >
	GREATER_EQUAL,  // >=
	EQUAL,			// ==
	NOT_EQUAL,      // !=
	AND,			// &&
	OR,				// ||
	NOT,			// !
	SEMICOLON,		// ;
	IDENTIFIER,     // identifier
	BITWISE_AND,	// &
	BITWISE_OR,		// |
};

class Token 
{
private:
	Tag tag_;	// 标签
	std::string value_; // 单词的值
	std::string get_tag_string()
	{
		switch (tag_)
		{
		case Tag::UNKNOWN:
			return "unknwon";
		case Tag::ERROR:
			return "error";
		case Tag::END_OF_FILE:
			return "end of file";
		case Tag::COMMENT:
			return "comment";
		case Tag::KW_INT:
			return "keyword int";
		case Tag::KW_CHAR:
			return "keyword char";
		case Tag::KW_STRING:
			return "keyword string";
		case Tag::KW_IF:
			return "keyword if";
		case Tag::KW_ELSE:
			return "keyword else";
		case Tag::KW_RETURN:
			return "keyword return";
		case Tag::KW_VOID:
			return "keyword void";
		case Tag::LT_NUMBER:
			return "literal number";
		case Tag::LT_CHAR:
			return "literal char";
		case Tag::LT_STRING:
			return "literal string";
		case Tag::LPAREN:
			return "(";
		case Tag::RPAREN:
			return ")";
		case Tag::LBRACES:
			return "{";
		case Tag::RBRACES:
			return "}";
		case Tag::LBRACKET:
			return "[";
		case Tag::RBRACKET:
			return "]";
		case Tag::ADD:
			return "+";
		case Tag::SUB:
			return "-";
		case Tag::MULTIPLY:
			return "*";
		case Tag::DIVIDE:
			return "/";
		case Tag::COMMA:
			return ",";
		case Tag::SEMICOLON:
			return ";";
		case Tag::ASSIGN:
			return "=";
		case Tag::EQUAL:
			return "==";
		case Tag::NOT_EQUAL:
			return "!=";
		case Tag::IDENTIFIER:
			return "identifier";
		case Tag::LESS:
			return "<";
		case Tag::LESS_EQUAL:
			return "<=";
		case Tag::GREATER:
			return ">";
		case Tag::GREATER_EQUAL:
			return ">=";
		case Tag::AND:
			return "&&";
		case Tag::OR:
			return "||";
		case Tag::NOT:
			return "!";
		default:
			return "unknown";
		}
	}
public:
	Token()
	{
		tag_ = Tag::UNKNOWN;
		value_ = "";
	}

	Token(Tag tag, std::string str) :tag_{ tag }, value_{str}
	{
	}

	Tag tag()
	{
		return tag_;
	}

	std::string get_name()
	{
		return value_;
	}

	virtual ~Token() {}

	virtual std::string to_string()
	{
		std::ostringstream ostr;
		ostr << "Tag: " << get_tag_string() << " string:" << value_;
		return std::string(ostr.str());
	}

	const static Token& unknown_token;
};

class Lexer
{
 public:
	Lexer();
	void scan(const char* file_name);
	char get_char();
	void unget_char();
	void reset();
	void load_file(const char* file_name);
	const Token& tokenize();

 private:
	Token* pcurrent_token_;
	std::string file_string_;
	unsigned int scan_pos_ = 0;
	std::string token_string_;

	void parse_number(char);
	void parse_word(char);
	void parse_character(char);
	void parse_comment(char ch);
	void skip_white_space();
	void parse_string(char);
};