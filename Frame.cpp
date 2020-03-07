#include "Frame.h"

/*
 * 在栈帧信息中插入一个变量，不可重复插入
 * name 变量名称
 * length 变量的长度
 */
void Frame::insert_variable(std::string name, unsigned char length)
{
    if (name.empty())
    {
        return;
    }
    if (length <= 0)
    {
        return;
    }
    if (var_offset_table.find(name) != var_offset_table.end())
    {
        return;
    }
    frame_offset_ -= length;
    var_offset_table[name] = frame_offset_;
}

/*
 * 返回变量是否存在
 */
bool Frame::is_variable_exists(std::string name)
{
    return (var_offset_table.find(name) != var_offset_table.end());
}

/*
 * 返回变量相对于栈帧地址的偏移长度，如果变量不存在，返回-1
 */
long Frame::get_variable_offset(std::string name)
{
    if (var_offset_table.find(name) != var_offset_table.end())
    {
        return var_offset_table[name];
    }
    else
    {
        return 0;
    }
}
