#pragma once
#include<string>
#include<fstream>
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