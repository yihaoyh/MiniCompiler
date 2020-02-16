#ifndef _UTILS_H_
#define _UTILS_H_
#include<string>
#include<fstream>
#include<iostream>
std::string read_string_from_file(const char* file_name);
bool is_blank(char ch);
bool is_digit(char ch);
void error(const char* content);
bool is_alphabet(char ch);
#endif // _UTILS_H_