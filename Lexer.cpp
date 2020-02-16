#include"Lexer.h"
#include"Utils.h"
#include"Common.h"
#include<iostream>
Lexer::Lexer()
{

}

void Lexer::scan(const char* file_name)
{
	if (file_string_.empty() && file_name != nullptr)
	{
		file_string_ = read_string_from_file(file_name);
	}
	char current_char = 0;
	std::string token_string = "";
	do
	{
		current_char = get_char();
		if (is_digit(current_char) && current_char != '0')
		{
			parse_number(current_char);
		}
		else if (is_alphabet(current_char))
		{
			parse_word(current_char);
		}
		else if (current_char == '\'')
		{
			parse_character(current_char);
		}
		std::cout << p_current_token_->to_string() << std::endl;
	} while (current_char != EOF);
}

void Lexer::reset()
{
	file_string_ = "";
	scan_pos_ = 0;
}

char Lexer::get_char()
{
	if (scan_pos_ < file_string_.size())
	{
		return file_string_.c_str()[scan_pos_++];
	}
	return EOF;
}

void Lexer::parse_number(char ch)
{
	token_string.append(1, ch);
	do
	{
		ch = get_char();
		if (ch <= '9' && ch >= '1')
		{
			token_string.append(1, ch);
		}
		else if (is_blank(ch)) {
			p_current_token_ = new Token(LT_NUMBER, token_string);
			break;
		}
		else
		{
			error(std::string("expected num character but:").append(1, ch).c_str());
		}
	} while (ch != EOF);
	token_string = "";
}

void Lexer::parse_word(char ch)
{
	token_string.append(1, ch);
	do
	{
		ch = get_char();
		if (is_alphabet(ch) || is_digit(ch) || ch == '_')
		{
			token_string.append(1, ch);
		}
		else if (is_blank(ch))
		{
			Tag tag;
			if ((tag = get_keyword_tag(token_string)) != UNKNOWN)
			{
				p_current_token_ = new Token(tag, token_string);
			}
			else
			{
				p_current_token_ = new Token(IDENTIFIER, token_string);
			}
			break;
		}
		else
		{
			error(std::string("expected alphabet or number or \'_\' but:").append(1, ch).c_str());
		}
	} while (ch != EOF);
	token_string = "";
}

void Lexer::parse_character(char ch)
{
	token_string.append(1, ch);
	ch = get_char();
	if (ch == '\\')
	{
		ch = get_char();
		switch (ch)
		{
		case 'a':
		case 'b':
		case 'f':
		case 'n':
		case 'r':
		case 't':
		case 'v':
		case '\\':
		case '\'':
		case '\?':
		case '\0':
			token_string.append(1, ch);
			break;
		default:
			error(std::string("illegal character after \\").append(1, ch).c_str());
			break;
		}
	}
	else 
	{
		token_string.append(1, ch);
	}
	ch = get_char();
	if (ch == '\'') 
	{
		token_string.append(1, ch);
		p_current_token_ = new Token(LT_CHAR, token_string);
	}
	else {
		error(std::string("expected \' but").append(1, ch).c_str());
	}
	token_string = "";
}