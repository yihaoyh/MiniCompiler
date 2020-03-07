#include "Frame.h"

/*
 * ��ջ֡��Ϣ�в���һ�������������ظ�����
 * name ��������
 * length �����ĳ���
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
 * ���ر����Ƿ����
 */
bool Frame::is_variable_exists(std::string name)
{
    return (var_offset_table.find(name) != var_offset_table.end());
}

/*
 * ���ر��������ջ֡��ַ��ƫ�Ƴ��ȣ�������������ڣ�����-1
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
