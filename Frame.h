#pragma once
#include <map>
#include <string>
#include <vector>

#include "Param.h"
class Frame {
 public:
  const Frame* parent;
  std::vector<Var> param_list;
  std::string fun_name;
  Frame() { parent = nullptr; }

  Frame(const Frame* parent, const std::string& fun_name,
        const std::vector<Var>& param_list)
      : parent(parent), fun_name{fun_name} {
    this->param_list = param_list;
  }

  void add_local(std::string name, unsigned char length);  // 添加局部变量
  void add_param_in(std::string name, char offset);        // 添加参数
  void add_param_out(unsigned char length);                // 参数压栈时，偏移地址减小 
  void remove_param_out(unsigned char length);
  bool is_variable_exists(std::string name);
  int get_variable_offset(std::string name);
  int get_frame_length();

 private:
  std::map<std::string, int> var_offset_table;  // 变量偏移表
  int frame_offset_ = 0;                        // 相对bsp的偏移
};
