#include "Frame.h"
#include "Utils.h"
/*
 * ��ջ֡��Ϣ�в���һ�������������ظ�����
 * name ��������
 * length �����ĳ���
 */
void Frame::add_local(std::string name, unsigned char length)
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
    ���һ��������Ϣ
    name  ������
    offset ����ƫ��
 */
void Frame::add_param_in(std::string name, char offset)
{
    // ������ƫ������Ҫ����8
    if (offset <= 8)
    {
        error("the offset of parameter must greater than 8");
        return;
    }
    var_offset_table[name] = offset;

}

void Frame::add_param_out(unsigned char length)
{
    if (length <= 0)
    {
        return;
    }
    frame_offset_ -= length;
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

/*
    ��ȡջ֡�ĳ��ȣ���Ϊ��ű��ر��������˿ռ䣬���Թ����˳�ʱҪ�ͷ���Щ�ռ�
*/
long Frame::get_frame_length()
{
    return -frame_offset_;
}
