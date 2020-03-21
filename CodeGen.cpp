#include "CodeGen.h"
#include <sstream>
#include <assert.h>
#include "Common.h"
#include "Utils.h"

#define VAR_LENGTH 8
#define PARAM_OFFSET_BEGIN 16
#define LOCAL_OFFSET_BEGIN 0
const std::string RAX = "%rax";
const std::string RBX = "%rbx";
const std::string RCX = "%rcx";

std::string CodeGen::parse_functions(std::vector<Function> functions)
{
    std::string code = "";
    for (auto iter = functions.begin(); iter != functions.end(); ++iter)
    {
        function_ = &(*iter);
        curr_frame_ = Frame(nullptr, iter->name, iter->get_params());
        parse_params();
        code += parse_function(*function_);
    }
    return code;
}

std::string CodeGen::parse_function(Function& fun)
{
    std::vector<InterInstruction> inst_list = fun.get_instructions();
    std::string code = "";
    code += gen_header(curr_frame_.fun_name);
    code += gen_enter_proc();
    for (auto iter = inst_list.begin(); iter != inst_list.end(); ++iter)
    {
        code += parse_instruction(*iter);
    }
    code += gen_exit_proc();
    return code;
}

std::string CodeGen::parse_instruction(const InterInstruction& inst)
{
    Var result = get_var(inst.result);
    Var arg1 = get_var(inst.arg1);
    Var arg2 = get_var(inst.arg2);
    std::string code = "";
    if (!inst.label.empty())
    {
        code += inst.label + ":\n";
    }
    if (inst.type == Inst_Type::IF_JUMP)
    {
        return code + gen_if_jump(inst.op, result, arg1, arg2);
    }
    else if (inst.type == Inst_Type::IF_FALSE_JUMP)
    {
        return code + gen_if_false_jump(inst.op, result, arg1, arg2);
    }
    else if (inst.type == Inst_Type::JUMP)
    {
        return code + gen_jump(result);
    }
    // normal
    switch (inst.op)
    {
    case Operator::OP_ADD:
    case Operator::OP_SUB:
        if (result.tag != IDENTIFIER)
        {
            error("result must be lvalue in add/sub");
            return "";
        }
        if (arg1.tag != IDENTIFIER && arg1.tag != LT_NUMBER)
        {
            error("arg1 must be number or id in add/sub");
        }
        if (arg2.tag != IDENTIFIER && arg2.tag != LT_NUMBER)
        {
            error("arg2 must be number or id in add/sub");
        }
        return gen_low_op(inst.op, result, arg1, arg2);
    case Operator::OP_ASSIGN:
        if (result.tag != IDENTIFIER)
        {
            error("result must be lvalue in add/sub");
            return "";
        }
        return code + gen_assign(result, arg1);
    case Operator::OP_SET_PARAM:
        return code + gen_set_param(arg1);
    case Operator::OP_CALL:
        code = code + gen_call(result, inst.arg1.value);
        return code;
    case Operator::OP_RETURN:
        return code + gen_return(arg1);
    case Operator::OP_IF_JUMP:
        return code + gen_if_jump(inst.op, result, arg1, arg2);
    default:
        return "";
    }
}

std::string CodeGen::gen_header(const std::string& fun_name)
{
    if (fun_name.empty())
    {
        return "";
    }
    std::stringstream sstream;
    if (fun_name == "main")
    {
        sstream << ".LC0:\n" \
            "\t.string	\"%d\\n\"\n"\
            "\t.text\n";
    }
    else
    {
        sstream << "\t.text\n";
    }
    sstream << "\t.globl  " << fun_name << "\n";
    sstream << fun_name << ":" << "\n";
    return sstream.str();
}

/*
 * ���ɸ�ֵָ�Ϊ���ٹ�������ֻ֧��64λ����
 * lval ��ֵ����rbx�Ĵ���
 * rval ��ֵ����rcx�Ĵ�������������
 * �ؼ�ָ����ʽ���� mov %rcx, %rbx ���� mov $10, %rbx
*/
std::string CodeGen::gen_assign(const Var& lval, const Var& rval)
{
    // �ж�rval�Ǳ�������������
    // ������������������������ַ�����ʽ�� str_rval = "$10"
    // ����Ǳ��������жϸñ�����ջ֡��Ϣ���Ƿ���ڡ�
    // ������ڣ�������һ��ָ���ָ����ݱ�����ƫ�Ƶ�ַ�����뵽ecx�Ĵ����У���ʹstr_rval = "%rcx"
    // ��������ڣ��򷵻�
    // ����ָ�� movq $str_rval, %rbx
    // Ȼ���ж�lval�Ƿ������ջ֡��Ϣ�У�������ڣ�������ָ�rbxд�뵽��Ӧ�ڴ��С�
    // ��������ڣ�������ָ�� push rbx��������Frame::insert_variable��ջ֡��Ϣ���һ������
    std::string code = "";
    std::string str_rval = "";
    if (lval.tag != IDENTIFIER)
    {
        return "";
    }
    code += gen_access_arg(rval, RBX);
    if (curr_frame_.is_variable_exists(lval.name))
    {
        long offset = curr_frame_.get_variable_offset(lval.name);
        if (!frame_offset_check(lval.name, offset))
        {
            return "";
        }
        code += gen_save_variable(RBX, offset);
    }
    else
    {
        code += gen_create_variable(RBX);
        curr_frame_.add_local(lval.name, get_type_length(lval.type));
    }
    return code;
}

/*
 * ���ɼ�/����ָ�Ϊ���ٹ�������ֻ֧��64λ����
 * result ���������Ϊ����
 * lval ��ֵ
 * rval ��ֵ
 */
std::string CodeGen::gen_low_op(Operator op, const Var& result, const Var& lval, const Var& rval)
{
    // �ж�result��tag�Ƿ�ΪID�������ǣ��򷵻�
    // ���lval����������������str_lvalΪ"$number"
    // �������ջ֡��Ϣ������һ��ָ���ָ����ڴ��ж�ȡlval��ֵ��rbx, str_lval����Ϊ"%rbx"����ջ֡�в����ڸ���Ϣ���򷵻ء�
    // ͬ������rval��ֻ�����Ĵ�����Ϊrcx.
    // ����ָ��add %rbx, %rcx
    // ��ջ֡��Ϣ�в���result����������ڣ�������һ������������ָ�������ڣ�������һ����ջд���ָ�
    std::string code = "";
    std::string str_lval = "";
    std::string str_rval = "";
    std::string str_op;
    if (!(op == Operator::OP_ADD || op == Operator::OP_SUB))
    {
        error("invalid operator");
        return "";
    }
    if (result.tag != IDENTIFIER)
    {
        error("result must be identifier");
        return "";
    }
    code += gen_access_arg(lval, RBX);
    code += gen_access_arg(rval, RCX);
    if (op == Operator::OP_ADD)
    {
        str_op = "\taddq";
    }
    else
    {
        str_op = "\tsubq";
    }
    code += str_op + " %rcx, %rbx\n";
    if (curr_frame_.is_variable_exists(result.name))
    {
        long offset = curr_frame_.get_variable_offset(result.name);
        if (offset == 0)
        {
            std::stringstream sstream;
            sstream << "var " << result.name << " offset is 0";
            print_error(sstream.str().c_str());
            return "";
        }
        code += gen_save_variable(RBX, offset);
    }
    else
    {
        code += gen_create_variable(RBX);
        curr_frame_.add_local(result.name, get_type_length(result.type));
    }
    return code;
}

/*
    ���ɽ������ʱ�Ĵ���
*/
std::string CodeGen::gen_enter_proc()
{
    return "\tpushq %rbp\n"\
        "\tmovq %rsp, %rbp\n";
}

/*
    �����˳�����ʱ�Ĵ��룬Ҫ��ջ�˳�
*/
std::string CodeGen::gen_exit_proc()
{
    std::stringstream sstream;
    sstream << ".end:\n";
    sstream << "\taddq $" << curr_frame_.get_frame_length() << ", %rsp\n";
    sstream << "\tpopq %rbp\n"\
        "\tret\n";
    return sstream.str();
}

/*
    ����return����ֵ����ֵ����rax��
*/
std::string CodeGen::gen_return(const Var& result)
{
    return gen_access_arg(result, RAX);
}

/*
 * ����һ�����ڴ�������������Ĵ�����ָ��
 * reg_name �Ĵ������� {"%rbx", "%rcx"}
 * offset �����bsp��ƫ�Ƶ�ַ
 */
std::string CodeGen::gen_load_variable(const std::string& reg_name, long offset)
{
    if (!register_check(reg_name))
    {
        return "";
    }
    if (!frame_offset_check(reg_name, offset))
    {
        return "";
    }
    std::stringstream sstream;
    sstream << "\tmovq " << offset << "(%rbp), " << reg_name << "\n";
    return sstream.str();
}

/*
 * ����һ�����������Ļ��ָ�ʵ�ַ�ʽ�ǽ�����ѹջ������ pushq %rbx
 */
std::string CodeGen::gen_create_variable(const std::string& reg_name)
{
    if (!register_check(reg_name))
    {
        return "";
    }
    std::stringstream sstream;
    sstream << "\tpushq " << reg_name << "\n";
    return sstream.str();
}

/*
    ������������ջ��
*/
std::string CodeGen::gen_save_variable(const std::string& reg_name, long offset)
{
    if (!register_check(reg_name))
    {
        return "";
    }
    if (!frame_offset_check(reg_name, offset))
    {
        return "";
    }
    std::stringstream sstream;
    sstream << "\tmovq " << reg_name << ", " << offset << "(%rbp)\n";
    return sstream.str();
}

std::string CodeGen::gen_call(const Var& result, const std::string& fun_name)
{
    if (fun_name == "print")
    {
        return gen_print_int(0);
    }
    std::stringstream sstream;
    sstream << "\tcall " << fun_name << "\n";
    if (result.tag == IDENTIFIER)
    {
        sstream << "\tmovq %rax, %rbx\n";
        sstream << gen_create_variable(RBX);
        curr_frame_.add_local(result.name, get_type_length(result.type));
    }
    return sstream.str();
}

std::string CodeGen::gen_set_param(const Var& param)
{
    if (param.tag != LT_NUMBER && param.tag != IDENTIFIER)
    {
        std::string error_msg = "tag of param " + param.name + " must be LT_NUMBER or IDENTIFIER";
        error(error_msg.c_str());
        return "";
    }
    std::string code = "";
    code += gen_access_arg(param, RBX);
    code += "\tpushq %rbx\n";
    curr_frame_.add_param_out(get_type_length(param.type));
    return code;
}

/*
    ��������Ϊif arg1 op arg2 goto result�����
*/
std::string CodeGen::gen_if_jump(Operator op, const Var& result, const Var& arg1, const Var& arg2)
{
    /*
        ���裺
        1 ����arg1��rbx��arg2��rcx
        2 ����cmpq rbx, rcx
        3 ���ݲ���������je��jge
    */
    std::string code = "";
    code += gen_access_arg(arg1, RBX);
    code += gen_access_arg(arg2, RCX);
    code += "\tcmpq " + RBX + ", " + RCX + "\n";
    std::string jump = "";
    switch (op)
    {
    case Operator::OP_LESS:
        jump = "jl";
        break;
    case Operator::OP_LESS_EQUAL:
        jump = "jle";
        break;
    case Operator::OP_EQUAL:
        jump = "je";
        break;
    case Operator::OP_NOT_EQUAL:
        jump = "jne";
        break;
    case Operator::OP_GREATER:
        jump = "jg";
        break;
    case Operator::OP_GREATER_EQUAL:
        jump = "jge";
        break;
    default:
        break;
    }
    std::string label = result.value_string;
    if (label.empty())
    {
        error("label is empty");
        return "";
    }
    code += "\t" + jump + " " + label + "\n";
    return code;
}

std::string CodeGen::gen_if_false_jump(Operator op, const Var& result, const Var& arg1, const Var& arg2)
{
    // ˼·ͬgen_if_jump��������תָ���Ƿ���
    std::string code = "";
    code += gen_access_arg(arg1, RBX);
    code += gen_access_arg(arg2, RCX);
    code += "\tcmpq " + RBX + ", " + RCX + "\n";
    std::string jump = "";
    switch (op)
    {
    case Operator::OP_LESS:
        jump = "jge";
        break;
    case Operator::OP_LESS_EQUAL:
        jump = "jg";
        break;
    case Operator::OP_EQUAL:
        jump = "jne";
        break;
    case Operator::OP_NOT_EQUAL:
        jump = "je";
        break;
    case Operator::OP_GREATER:
        jump = "jle";
        break;
    case Operator::OP_GREATER_EQUAL:
        jump = "jl";
        break;
    default:
        break;
    }
    std::string label = result.value_string;
    if (label.empty())
    {
        error("label is empty");
        return "";
    }
    code += "\t" + jump + " " + label + "\n";
    return code;
}

std::string CodeGen::gen_jump(const Var& result)
{
    std::string code = "";
    std::string label = result.value_string;
    if (label.empty())
    {
        error("label is empty");
        return "";
    }
    code += "\tjmp " + label + "\n";
    return code;
    return std::string();
}

bool CodeGen::register_check(const std::string& reg_name)
{
    if (reg_name == RBX || reg_name == RCX || reg_name == RAX)
    {
        return true;
    }
    std::string error_msg = "invalid register name:" + reg_name;
    error(error_msg.c_str());
    return false;
}

/*
 * ͨ��ָ���ַ��ȡ����
 */
Var CodeGen::get_var(const Address& address)
{
    switch (address.type)
    {
    case NAME:
    case TEMP_VAR:
        return function_->get_variable(address.value);
    case LITERAL_NUMBER:
        return Var(LT_NUMBER, "", address.value, Type::INT);
    case LITERAL_CHAR:
        return Var(LT_CHAR, "", address.value, Type::CHAR);
    case LITERAL_STRING:
        return Var(LT_STRING, "", address.value, Type::STRING);
    default:
        return Var();
    }
}

/**
    ���ɷ��ʲ������Ļ����룬֧��ID�����γ���
    var ������
    reg_name �Ĵ������� {"%rax", "%rbx", "%rcx"}
*/
std::string CodeGen::gen_access_arg(const Var& var, const std::string& reg_name)
{
    std::string error_msg = "";
    if (!register_check(reg_name))
    {
        return "";
    }
    std::string str_val;
    if (var.tag == LT_NUMBER)
    {
        str_val = "$" + var.value_string;
        return "\tmovq " + str_val + " , " + reg_name + "\n";
    }
    else if (var.tag == IDENTIFIER)
    {
        if (!variable_exist_check(var.name))
        {
            return "";
        }
        long offset = curr_frame_.get_variable_offset(var.name);
        if (!frame_offset_check(var.name, offset))
        {
            return "";
        }
        return gen_load_variable(reg_name, offset);
    }
    return "";
}

/*
    ��ÿ��������ƫ�Ƶ�ַ�洢��ջ֡���У��Ա������ȡ��
    ��Ϊ������ʼִ��֮ǰ���ù�call(���ʾ���push rip)��push rbp��
    ���Բ�����ƫ����ʼ��ַΪrbp+16��
*/
void CodeGen::parse_params()
{
    assert(function_ != nullptr);
    int offset = PARAM_OFFSET_BEGIN;
    const std::vector<Var> params = function_->get_params();
    for (unsigned int i = 0; i < params.size(); i++)
    {
        Var var = params[i];
        curr_frame_.add_param_in(var.name, offset);
        int length = get_type_length(var.type);
        offset += length;
    }
}

/*
    ��ȡ���͵ĳ��ȣ���λΪ�ֽ�
*/
int CodeGen::get_type_length(const Type& type)
{
    switch (type)
    {
    case Type::INT:
        return 8;
    }
    return 0;
}

bool CodeGen::variable_exist_check(const std::string& var_name)
{
    if (!curr_frame_.is_variable_exists(var_name))
    {
        std::string error_msg = "var " + var_name + " not found in stackframe";
        error(error_msg.c_str());
        return false;
    }
    return true;
}

bool CodeGen::frame_offset_check(const std::string& var_name, long offset)
{
    if (offset >= LOCAL_OFFSET_BEGIN && offset < PARAM_OFFSET_BEGIN)
    {
        std::stringstream sstream;
        sstream << "var " << var_name << " offset is " << offset << ",it can't be in [0,16)";
        error(sstream.str().c_str());
        return false;
    }
    return true;
}