#include "Frame.h"

#include "Utils.h"
/*
 * 在栈帧信息中插入一个变量，不可重复插入
 * name 变量名称
 * length 变量的长度
 */
void Frame::add_local(std::string name, unsigned char length) {
  if (name.empty()) {
    return;
  }
  if (length <= 0) {
    return;
  }
  if (var_offset_table.find(name) != var_offset_table.end()) {
    return;
  }
  frame_offset_ -= length;
  var_offset_table[name] = frame_offset_;
}

/*
    添加一个参数信息
    name  参数名
    offset 参数偏移
 */
void Frame::add_param_in(std::string name, char offset) {
  // 参数的偏移量需要大于8
  if (offset <= 8) {
    error("the offset of parameter must greater than 8");
    return;
  }
  var_offset_table[name] = offset;
}

void Frame::add_param_out(unsigned char length) {
  if (length <= 0) {
    return;
  }
  frame_offset_ -= length;
}

/*
 * 返回变量是否存在
 */
bool Frame::is_variable_exists(std::string name) {
  return (var_offset_table.find(name) != var_offset_table.end());
}

/*
 * 返回变量相对于栈帧地址的偏移长度，如果变量不存在，返回-1
 */
int Frame::get_variable_offset(std::string name) {
  if (var_offset_table.find(name) != var_offset_table.end()) {
    return var_offset_table[name];
  } else {
    return 0;
  }
}

/*
    获取栈帧的长度，因为存放本地变量申请了空间，所以过程退出时要释放这些空间
*/
int Frame::get_frame_length() { return -frame_offset_; }
