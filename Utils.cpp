#include"Utils.h"
#include<sstream>
#include<windows.h>
#include<cstdio>

#define BLOCK_SIZE 4096
/*
* 从文件中读取字符串
*/
std::string read_string_from_file(const char* file_name)
{
	std::string result = "";
	char buffer[BLOCK_SIZE];
	std::fstream out_file;
	out_file.open(file_name, std::ios::in);
	while (!out_file.eof())
	{
		memset(buffer, 0, BLOCK_SIZE);
		out_file.getline(buffer, BLOCK_SIZE, '\n');
		result = result + std::string(buffer) + "\n";
	}
	out_file.close();
	return result;
}

bool write_string_to_file(const char* file_name, const std::string content)
{
	remove(file_name);
	std::fstream out_file;
	out_file.open(file_name, std::ios::out|std::ios::app);
	out_file.write(content.c_str(), content.size());
	out_file.close();
    return true;
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
	// TODO 下面代码和push rbp似乎有冲突，会引发段错误
	std::stringstream sstream;
	sstream << "\tmovl %ebx" << ", %esi\n";
	sstream << "\tleaq	.LC0(%rip), %rdi\n";
	sstream << "\tcall	printf@PLT\n";
	return std::string();
}
