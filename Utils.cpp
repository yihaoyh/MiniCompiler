#include"Utils.h"
#include<sstream>
#include<windows.h>
/*
* 从文件中读取字符串
*/
std::string read_string_from_file(const char* file_name)
{
	std::string result = "";
	char buffer[256];
	std::fstream out_file;
	out_file.open(file_name, std::ios::in);
	while (!out_file.eof())
	{
		memset(buffer, 0, 256);
		out_file.getline(buffer, 256, '\n');
		result = result + std::string(buffer) + "\n";
	}
	out_file.close();
	return result;
}

bool is_blank(char ch)
{
	if (ch == ' ' || ch == '\n' || ch == '\t') {
		return true;
	}
	return false;
}

/*
	switch (current_state)
	{
	case KW_CHAR:
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);
		break;
	case TK_CINT:
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
		break;
	default:
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
		break;
	}
	if (current_state <= KW_SIZEOF && current_state >= KW_CHAR)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE);
	}*/

void error(const char* content)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
	std::cout << content << std::endl;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
}

bool is_digit(char ch)
{
	if (ch >= '0' && ch <= '9')
		return true;
	return false;
}

bool is_alphabet(char ch)
{
	if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'))
	{
		return true;
	}
	return false;
}

std::string gen_print_int(int value)
{
	std::stringstream sstream;
	sstream << "movl $" << value << ", %esi\n";
	sstream << "leaq	.LC0(%rip), %rdi\n";
	sstream << "call	printf@PLT\n";
	return std::string();
}
