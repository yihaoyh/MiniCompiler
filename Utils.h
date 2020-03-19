#ifndef _UTILS_H_
#define _UTILS_H_
#include<string>
#include<fstream>
#include<iostream>
std::string read_string_from_file(const char* file_name);
bool write_string_to_file(const char* file_name, const std::string content);
bool is_blank(char ch);
bool is_digit(char ch);
void debug(const char* content);
void error(const char* content);
bool is_alphabet(char ch);
std::string gen_print_int(int value);
#endif // _UTILS_H_