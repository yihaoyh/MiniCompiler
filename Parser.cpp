#include "Parser.h"
#include<string>
#include<iostream>
#include<assert.h>
#include"CodeGen.h"
#include"Utils.h"

#define FIRST(TAG) token_look_.tag() == TAG
#define OR(TAG) ||token_look_.tag() == TAG
#define TYPE_FIRST FIRST(KW_INT)OR(KW_CHAR)OR(KW_STRING)OR(KW_VOID)
#define EXPR_FIRST FIRST(LPAREN)OR(LT_NUMBER)OR(LT_CHAR)OR(LT_STRING)OR(IDENTIFIER)OR(SUB)OR(MULTIPLY)
#define STATEMENT_FIRST EXPR_FIRST OR(KW_IF)OR(KW_RETURN)
Parser::Parser()
{
    current_function_ = nullptr;
    index = -1;
}
void Parser::begin_parse(const std::string& file_name)
{
    if (file_name.empty())
    {
        return;
    }
    lexer_.load_file(file_name.c_str());
    move_token();
    program();
}
void Parser::print_instructions()
{
    std::vector<Function> functions = get_functions();
    CodeGen code_gen = CodeGen();
    for (unsigned int i = 0; i < functions.size(); i++)
    {
        Function fun = functions[i];
        std::cout << "function " << fun.name << " begin" << std::endl;
        std::vector<InterInstruction> insts = fun.get_instructions();
        for (unsigned int j = 0; j < insts.size(); j++)
        {
            std::cout << "\t" << insts[j].to_string() << std::endl;
        }
        std::cout << "function " << fun.name << " end" << std::endl;
    }
    std::cout << "\n" << "code:\n";
    std::cout << code_gen.parse_functions(functions);
}
void Parser::program()
{
    //move_token();
    if (FIRST(END_OF_FILE))
    {
        return;
    }
    else
    {
        segment();
        program();
    }
}

void Parser::segment()
{
    Type t = type();
    move_token();
    def(t);
}

Type Parser::type()
{
    if (TYPE_FIRST)
    {
        return tag_to_type(token_look_.tag());
    }
    else
    {
        recovery();
        return Type::UNKNOWN;
    }
}
// <def>->ID <idtail> 代码形式如 " a = a + b"
void Parser::def(const Type& type)
{
    std::string name = "";
    if (FIRST(IDENTIFIER))
    {
        name = token_look_.get_name();
        move_token();
    }
    else
    {
        recovery();
    }
    Var var = idtail(type, name);
}

Var Parser::varrdef(Var result)
{
    return init(result);
}

// <init>-> ASSIGN <expr> | ε  代码"= a * b"
Var Parser::init(Var result)
{
    if (result.tag != IDENTIFIER)
    {
        recovery();
        error("result must be lvalue");
        return Var::default_;
    }
    Var var;
    if (match(ASSIGN))
    {
        var = expr();
        if (var.tag != IDENTIFIER && var.tag != LT_NUMBER && var.tag != LT_CHAR && var.tag != LT_STRING)
        {
            recovery();
            error("arg1 must be id or literal");
            return Var::default_;
        }
        add_instruction(InterInstruction(var_to_address(result), Operator::OP_ASSIGN, var_to_address(var), Address()));
    }
    else if (FIRST(END_OF_FILE))
    {
        return var;
    }
    else {
        recovery();
    }
    return var;
}

void Parser::deflist()
{
    //move_token();
}

// <defdata>->ID <varrdef> 对应代码"a = 10 * b"
void Parser::defdata(const Type& type)
{
    if (FIRST(IDENTIFIER))
    {
        std::string name = token_look_.get_name();
        move_token();
        Var result;
        result.tag = IDENTIFIER;
        result.name = name;
        result.type = type;
        put_variable(result);
        Var arg1 = varrdef(result);
        //put_variable(arg1);

        //Operator op = Operator::OP_ASSIGN;
        //add_instruction(InterInstruction(result, op, arg1, Var::default_));
    }
}

void Parser::paradata(std::vector<Var>& params)
{
    if (TYPE_FIRST)
    {
        Type type = tag_to_type(token_look_.tag());
        move_token();
        if (FIRST(IDENTIFIER))
        {
            std::cout << "para name " << token_look_.get_name() << std::endl;
            // 将参数类型添加到函数
            params.push_back(Var::create_id(token_look_.get_name(), type));
            move_token();
        }
    }
}

void Parser::paradatatail()
{
}

void Parser::para(std::vector<Var>& params)
{
    paradata(params);
    if (match(COMMA))
    {
        para(params);
    }
}

void Parser::block()
{
    if (FIRST(LBRACES))
    {
        move_token();
        subprogram();
        move_token();
        if (FIRST(RBRACES))
        {
            return;
        }
        else
        {
            recovery();
        }
    }
}

void Parser::subprogram()
{
    // <subprogram>-><localdef> <subprogram> | <statement> <subprogram> | ε
    if (FIRST(END_OF_FILE))
    {
        return;
    }
    else if (TYPE_FIRST)
    {
        localdef();
        subprogram();
    }
    else if (STATEMENT_FIRST)
    {
        statement();
        subprogram();
    }
}

//<expr> <expr> SEMICOLON | KW_RETURN <expr> SEMICOLON
void Parser::statement()
{
    if (FIRST(KW_RETURN))
    {
        move_token();
        Var result = expr();
        if (FIRST(SEMICOLON))
        {
            add_instruction(InterInstruction(Address(), Operator::OP_RETURN, var_to_address(result), Address()));
            return;
        }
        else
        {
            recovery();
        }
    }
    else
    {
        Var var = expr();
        put_variable(var);
        if (match(SEMICOLON))
        {
            return;
        }
        else
        {
            recovery();
        }
    }
}

/*
 * 赋值表达式，<assign_expr> -> alo_expr assign_tail  code:a = a + b;
 */
Var Parser::assign_expr()
{
    Var lvalue = alo_expr();
    assign_tail(lvalue);
    return lvalue;
}

void Parser::assign_tail(Var lvalue)
{
    if (match(ASSIGN))
    {
        Var rvalue = assign_expr();
        if (lvalue.tag != IDENTIFIER)
        {
            error("assignment must with a lvalue");
            recovery();
            return;
        }
        if (rvalue.tag != IDENTIFIER && rvalue.tag != LT_NUMBER && rvalue.tag != LT_CHAR && rvalue.tag != LT_STRING)
        {
            recovery();
            error("rvalue must be id or literal");
            return;
        }
        add_instruction(InterInstruction(var_to_address(lvalue), Operator::OP_ASSIGN, var_to_address(rvalue), Address()));
    }
}
// <localdef>-><type> <defdata> <deflist>
void Parser::localdef()
{
    Type t = type();
    move_token();
    defdata(t);
    //move_token();
    deflist();
    if (FIRST(SEMICOLON))
    {
        move_token();
    }
    else
    {
        recovery();
    }
}

/*
    <expr>-><item> <exprtail> 对应代码 "a + b","a = 10"
    此处因为expr和assign混用，导致一系列问题，需要重构
*/
Var Parser::expr()
{
    return assign_expr();
}


void Parser::operator_()
{
}

void Parser::operand()
{
}

Operator Parser::op_low()
{
    if (FIRST(ADD)OR(SUB))
    {
        switch (token_look_.tag())
        {
        case ADD:
            return Operator::OP_ADD;
        case SUB:
            return Operator::OP_SUB;
        }
    }
    recovery();
    return Operator::OP_INVALID;
}
Var Parser::alo_expr()
{
    Var var = item();
    return alo_tail(var);
}

/*
    加减运算的尾部
    arg1 表示运算符左边的变量
    <alo_tail> -> {+|-} <item> <alo_tail>  code: + a + b;
*/
Var Parser::alo_tail(const Var& arg1)
{
    assert(current_function_ != nullptr);
    if (FIRST(ADD)OR(SUB))
    {
        Operator op = op_low();
        move_token();
        Var arg2 = item();
        Var temp = current_function_->gen_temp_var(arg1.type);
        add_instruction(InterInstruction(var_to_address(temp), op, var_to_address(arg1), var_to_address(arg2)));
        return alo_tail(temp);
    }
    return arg1;
}
bool Parser::match(const Tag& tag)
{
    if (token_look_.tag() == tag)
    {
        move_token();
        return true;
    }
    else
    {
        return false;
    }
}
void Parser::para_transit()
{
    Var var = expr();
    // 生成一个表达式的指令，再生成一个保存到参数列表的指令，这里传的是形参
    // 保存到参数列表在汇编中的实现是将表达式结果压栈（无论该变量之前是否已经保存在栈中）。
    // 因为通用的方式是将参数放在调用函数栈帧的底部
    std::cout << "parse one param" << std::endl;
    if (match(COMMA))
    {
        para_transit();
    }
    InterInstruction inst = InterInstruction(Address(), Operator::OP_SET_PARAM, var_to_address(var), Address());
    add_instruction(inst);
}
// 对应代码 10 * a
Var Parser::item()
{
    Var var = factor();
    move_token();
    var = itemtail(var);
    //std::cout << "item " << token_look_.get_name() << std::endl;
    return var;
}

/*
   乘除运算尾部
   <itemtail>-><op_high> <factor> <itemtail>  对应代码 * b * c
*/
Var Parser::itemtail(const Var& arg1)
{
    assert(current_function_ != nullptr);
    if (FIRST(MULTIPLY)OR(DIVIDE))
    {
        Operator op = op_high();
        move_token();
        Var arg2 = factor();
        Var result = current_function_->gen_temp_var(arg1.type);
        add_instruction(InterInstruction(var_to_address(result), op, var_to_address(arg1), var_to_address(arg2)));
        move_token();
        return itemtail(result);
    }
    return arg1;
}

Operator Parser::op_high()
{
    if (FIRST(MULTIPLY)OR(DIVIDE))
    {
        switch (token_look_.tag())
        {
        case MULTIPLY:
            return Operator::OP_MUL;
        case DIVIDE:
            return Operator::OP_DIV;
        }
    }
    recovery();
    return Operator::OP_INVALID;
}

/* <factor> -> val factor()会多读一个token */
Var Parser::factor()
{
    return val();
}

/* <val> -> elem */
Var Parser::val()
{
    return elem();
}

/* <elem>-> ID |ID()| (expr) | literal   elem() 会多读一个token*/
Var Parser::elem()
{
    Var var;
    if (FIRST(IDENTIFIER))
    {
        // 从符号表中读取变量
        std::string name = token_look_.get_name();
        move_token();
        var = idexpr(name);

    }
    else if (FIRST(LPAREN))
    {
        move_token();
        var = expr();
        move_token();
        if (!(FIRST(RPAREN))) {
            recovery();
        }
    }
    else
    {
        var = literal();
    }
    return var;
}

Var Parser::literal()
{
    if (FIRST(LT_NUMBER)OR(LT_CHAR)OR(LT_STRING))
    {
        Type type = tag_to_type(token_look_.tag());

        if (type == Type::UNKNOWN)
        {
            error("can not parse the type of literal:");
            recovery();
        }
        else
        {
            Var var = Var::create_number(token_look_.get_name());
            return var;
        }
    }
    else
    {
        recovery();
    }
    return Var();

}

void Parser::recovery()
{
    std::cout << "recovery wrong token " << token_look_.get_name() << std::endl;
}

void Parser::move_token()
{
    if (index == token_stack_.size() - 1)
    {
        do
        {
            token_look_ = lexer_.tokenize();
        } while (token_look_.tag() == COMMENT);
        token_stack_.push_back(token_look_);
        index++;
    }
    else {
        token_look_ = token_stack_[++index];
    }
}

/*
    id尾部，可能为函数调用或者是变量定义
    <idtail>-><varrdef> <deflist> | LPAREN <para> RPAREN <funtail>
*/
Var Parser::idtail(const Type& type, std::string name)
{
    Var var;
    if (match(LPAREN))
    {
        std::vector<Var> params;
        para(params);
        if (!(FIRST(RPAREN)))
        {
            recovery();
        }
        move_token();
        funtail(type, name, params);
    }
    else {
        Var result = Var::create_id(name, type);
        var = varrdef(result);
        move_token();
        deflist();
        put_variable(var);
    }
    return var;
}

void Parser::funtail(const Type& type, std::string fun_name, const std::vector<Var>& params)
{
    if (FIRST(SEMICOLON))
    {
        std::cout << "function declaration occur name:" << fun_name << std::endl;
        Function func = Function(fun_name, true, type, std::vector<Var>());
        functions_.push_back(func);
        put_function(func);
    }
    else if (FIRST(LBRACES)) {
        std::cout << "function definition occur name:" << fun_name << std::endl;
        Function func = Function(fun_name, false, type, params);
        functions_.push_back(func);
        put_function(func);
        current_function_ = &functions_.back();
        block();
        current_function_ = nullptr;
    }
    move_token();
}

/*
    id的表达式
*/
Var Parser::idexpr(std::string name)
{
    assert(current_function_ != nullptr);

    Var var;
    if (match(LPAREN))
    {
        // 处理传参
        para_transit();
        if (FIRST(RPAREN) == false)
        {
            recovery();
        }

        // 函数调用
        Function fun = get_function(name);
        if (!fun.name.empty())
        {
            // 从右往左压参数，可以支持不定长参数，所以不需要传参数个数
            std::cout << "function call occur : " << name << "()" << std::endl;
            var = current_function_->gen_temp_var(fun.return_type);
            InterInstruction inst = InterInstruction(var_to_address(var), Operator::OP_CALL, Address{ NAME, name }, Address());
            add_instruction(inst);
        }
    }
    else
    {
        var = current_function_->get_variable(name);
        back_token();
    }
    return var;
}

void Parser::put_variable(Var var)
{
    assert(current_function_ != nullptr);
    current_function_->put_variable(var);
}

void Parser::add_instruction(InterInstruction instrunction)
{
    assert(current_function_ != nullptr);
    current_function_->add_instruction(instrunction);
}

/*
 * 向后移动一个token
 */
void Parser::back_token()
{
    if (index > 0)
    {
        token_look_ = token_stack_[--index];
    }
}

Function Parser::get_function(std::string name)
{
    if (function_table_.find(name) != function_table_.end())
    {
        return function_table_[name];
    }
    return Function();
}

void Parser::put_function(Function fun)
{
    function_table_[fun.name] = fun;
}

std::vector<Function> Parser::get_functions()
{
    return functions_;
}