#include "Parser.h"

#include <assert.h>

#include <iostream>
#include <string>
#include <vector>

#include "CodeGen.h"
#include "Utils.h"

#define FIRST(TAG) token_look_.tag() == TAG
#define OR(TAG) || token_look_.tag() == TAG
#define TYPE_FIRST \
  FIRST(Tag::KW_INT) OR(Tag::KW_CHAR) OR(Tag::KW_STRING) OR(Tag::KW_VOID)
#define EXPR_FIRST   \
  FIRST(Tag::LPAREN) \
  OR(Tag::LT_NUMBER) \
  OR(Tag::LT_CHAR)   \
  OR(Tag::LT_STRING) OR(Tag::IDENTIFIER) OR(Tag::SUB) OR(Tag::MULTIPLY)
#define STATEMENT_FIRST \
  EXPR_FIRST OR(Tag::KW_IF) OR(Tag::KW_RETURN) OR(Tag::KW_WHILE)
#define COMPARE_FIRST \
  FIRST(Tag::LESS)    \
  OR(Tag::LESS_EQUAL) \
  OR(Tag::EQUAL) OR(Tag::NOT_EQUAL) OR(Tag::GREATER) OR(Tag::GREATER_EQUAL)

Parser::Parser() {
  current_function_ = nullptr;
  index = -1;
  // 预添加print函数
  Function fun = Function("print", false, Type::VOID, std::vector<Var>());
  function_table_["print"] = fun;
}
void Parser::begin_parse(const std::string& file_name) {
  if (file_name.empty()) {
    return;
  }
  lexer_.load_file(file_name.c_str());
  move_token();
  program();
}

// 对生成的指令进行后处理，给指令添加标号，消除不必要的goto，例如当goto的目标是自己的下一条语句时，去除goto
void Parser::post_parse() {
  for (auto iter = functions_.begin(); iter != functions_.end(); ++iter) {
    std::vector<InterInstruction>& insts = iter->get_instructions();
    for (auto inst_iter = insts.begin(); inst_iter != insts.end();
         ++inst_iter) {
      if (inst_iter->type == Inst_Type::IF_JUMP ||
          inst_iter->type == Inst_Type::IF_FALSE_JUMP ||
          inst_iter->type == Inst_Type::JUMP) {
        Address& addr = inst_iter->result;
        instr_number index = stoi(addr.value);
        if (index < 0 || index > insts.size()) {
          error("post_parse::invalid index");
          continue;
        }
        if (index == inst_iter->index + 1) {
          inst_iter->removed = true;
        } else {
          if (index == insts.size()) {
            inst_iter->result.value = ".end_" + iter->name;
          } else {
            InterInstruction& jump_dest = insts[index];
            if (jump_dest.label.empty()) {
              jump_dest.label = iter->gen_label();
            }
            inst_iter->result.value = jump_dest.label;
          }
        }
      }
    }

    for (auto inst_iter = insts.begin(); inst_iter != insts.end();) {
      if (inst_iter->removed) {
        inst_iter = insts.erase(inst_iter);
      } else {
        ++inst_iter;
      }
    }
  }
}

void Parser::print_instructions() {
  std::vector<Function> functions = get_functions();
  CodeGen code_gen = CodeGen();
  for (unsigned int i = 0; i < functions.size(); i++) {
    Function fun = functions[i];
    std::cout << "function " << fun.name << " begin" << std::endl;
    std::vector<InterInstruction> insts = fun.get_instructions();
    for (unsigned int j = 0; j < insts.size(); j++) {
      std::cout << "\t" << insts[j].to_string() << std::endl;
    }
    std::cout << "function " << fun.name << " end" << std::endl;
  }
  std::cout << "\n"
            << "code:\n";
  std::string code = code_gen.parse_functions(functions);
  write_string_to_file("new_code.s", code);
}
void Parser::program() {
  // move_token();
  if (FIRST(Tag::END_OF_FILE)) {
    return;
  } else {
    segment();
    program();
  }
}

void Parser::segment() {
  Type t = type();
  move_token();
  def(t);
}

Type Parser::type() {
  if (TYPE_FIRST) {
    return tag_to_type(token_look_.tag());
  } else {
    recovery();
    return Type::UNKNOWN;
  }
}
// <def>->ID <idtail> 代码形式如 " a = a + b"
void Parser::def(const Type& type) {
  std::string name = "";
  if (FIRST(Tag::IDENTIFIER)) {
    name = token_look_.get_name();
    move_token();
  } else {
    recovery();
  }
  Var var = idtail(type, name);
}

Var Parser::varrdef(Var result) { return init(result); }

// <init>-> ASSIGN <expr> | ε  代码"= a * b"
Var Parser::init(Var result) {
  if (result.tag != Tag::IDENTIFIER) {
    recovery();
    error("result must be lvalue");
    return Var::default_;
  }
  Var var;
  if (match(Tag::ASSIGN)) {
    var = expr();
    if (var.tag != Tag::IDENTIFIER && var.tag != Tag::LT_NUMBER &&
        var.tag != Tag::LT_CHAR && var.tag != Tag::LT_STRING) {
      recovery();
      error("arg1 must be id or literal");
      return Var::default_;
    }
    add_instruction(&InterInstruction(var_to_address(result),
                                      Operator::OP_ASSIGN, var_to_address(var),
                                      Address()));
  } else if (FIRST(Tag::END_OF_FILE)) {
    return var;
  } else {
    recovery();
  }
  return var;
}

void Parser::deflist() {
  // move_token();
}

// <defdata>->ID <varrdef> 对应代码"a = 10 * b"
void Parser::defdata(const Type& type) {
  if (FIRST(Tag::IDENTIFIER)) {
    std::string name = token_look_.get_name();
    move_token();
    Var result;
    result.tag = Tag::IDENTIFIER;
    result.name = name;
    result.type = type;
    put_variable(result);
    Var arg1 = varrdef(result);
  }
}

void Parser::paradata(std::vector<Var>* params) {
  if (TYPE_FIRST) {
    Type type = tag_to_type(token_look_.tag());
    move_token();
    if (FIRST(Tag::IDENTIFIER)) {
      std::cout << "para name " << token_look_.get_name() << std::endl;
      // 将参数类型添加到函数
      params->push_back(Var::create_id(token_look_.get_name(), type));
      move_token();
    }
  }
}

void Parser::paradatatail() {}

void Parser::para(std::vector<Var>* params) {
  paradata(params);
  if (match(Tag::COMMA)) {
    para(params);
  }
}

Statement Parser::block() {
  Statement s;
  if (FIRST(Tag::LBRACES)) {
    move_token();
    s = subprogram();
    if (match(Tag::RBRACES)) {
      return s;
    } else {
      recovery();
    }
  }
  return s;
}

// <subprogram>-><localdef> <subprogram> | <statement> <subprogram> | ε
Statement Parser::subprogram() {
  if (FIRST(Tag::END_OF_FILE)) {
    return Statement();
  } else if (TYPE_FIRST) {
    localdef();
    return subprogram();
  } else if (STATEMENT_FIRST) {
    /*
        L -> S M L1
        backpatch(S.nextlist, M.instr)
        L.nextlist = L1.nextlist
    */
    Statement l;
    Statement s = statement();
    instr_number m = current_function_->get_next_instruction();
    BoolExpr::back_patch_list(current_function_, s.next_list, m);
    Statement l1 = subprogram();
    if (!l1.next_list.empty()) {
      l.next_list = l1.next_list;
    }
    return l;
  }

  return Statement();
}

// <expr> <expr> SEMICOLON | KW_RETURN <expr> SEMICOLON
Statement Parser::statement() {
  Statement s;
  if (match(Tag::KW_RETURN)) {
    Var result = expr();
    if (match(Tag::SEMICOLON)) {
      add_instruction(&InterInstruction(Address(), Operator::OP_RETURN,
                                        var_to_address(result), Address()));
      s.next_list.clear();
    } else {
      recovery();
    }
  } else if (match(Tag::KW_IF)) {
    /*
        S->if (B) M S1
        backpatch(B.truelist, M.instr)
        s.nextlist = merge(B.falselist, S1.nextlist)
    */
    if (match(Tag::LPAREN)) {
      BoolExpr b = or_expr();
      if (match(Tag::RPAREN)) {
        instr_number m = current_function_->get_next_instruction();
        b.back_patch(current_function_, BackPatchType::True, m);
        Statement s1 = block();
        s.next_list = merge(b.false_list, s1.next_list);
      } else {
        recovery();
      }
    } else {
      recovery();
    }
  } else if (match(Tag::KW_DO)) {
    // TODO 实现 do_while
    block();
    if (match(Tag::KW_WHILE)) {
      BoolExpr b = or_expr();
    }

  } else if (match(Tag::KW_WHILE)) {
    instr_number M1 = current_function_->get_next_instruction();
    if (match(Tag::LPAREN)) {
      BoolExpr B = or_expr();
      if (match(Tag::RPAREN)) {
        instr_number M2 = current_function_->get_next_instruction();
        Statement S1 = block();
        B.back_patch_list(current_function_, S1.next_list, M1);
        B.back_patch(current_function_, BackPatchType::True, M2);
        s.next_list = B.false_list;
        InterInstruction jump = InterInstruction::gen_jump();
        jump.result = Address{LITERAL_STRING, std::to_string(M1)};
        add_instruction(&jump);
      }
    }
  } else {
    Var var = expr();
    put_variable(var);
    if (match(Tag::SEMICOLON)) {
      s.next_list.clear();
    } else {
      recovery();
    }
  }
  return s;
}

/*
 * 赋值表达式，<assign_expr> -> alo_expr assign_tail  code:a = a + b;
 */
Var Parser::assign_expr() {
  Var lvalue = alo_expr();
  assign_tail(lvalue);
  return lvalue;
}

void Parser::assign_tail(Var lvalue) {
  if (match(Tag::ASSIGN)) {
    Var rvalue = assign_expr();
    if (lvalue.tag != Tag::IDENTIFIER) {
      error("assignment must with a lvalue");
      recovery();
      return;
    }
    if (rvalue.tag != Tag::IDENTIFIER && rvalue.tag != Tag::LT_NUMBER &&
        rvalue.tag != Tag::LT_CHAR && rvalue.tag != Tag::LT_STRING) {
      recovery();
      error("rvalue must be id or literal");
      return;
    }
    add_instruction(&InterInstruction(var_to_address(lvalue),
                                      Operator::OP_ASSIGN,
                                      var_to_address(rvalue), Address()));
  }
}

BoolExpr Parser::or_expr() {
  BoolExpr expr = and_expr();
  return or_tail(&expr);
}
BoolExpr Parser::or_tail(BoolExpr* B1) {
  if (match(Tag::OR)) {
    /*
        B -> B1 || M B2
        backpatch(B1.falselist, M.instr);
        B.truelist = merge(B1.truelist, B2.truelist);
        B.falselist = B2.falselist;
    */
    unsigned int M = current_function_->get_next_instruction();
    B1->back_patch(current_function_, BackPatchType::False, M);
    BoolExpr B, B2;
    B2 = and_expr();
    B.true_list = merge(B1->true_list, B2.true_list);
    B.false_list = B2.false_list;
    return B;
  }
  return *B1;
}

BoolExpr Parser::and_expr() {
  BoolExpr b1 = compare_expr();
  return and_tail(&b1);
}

BoolExpr Parser::and_tail(BoolExpr* B1) {
  if (match(Tag::AND)) {
    /*
        对应龙书的布尔表达式回填逻辑
        B -> B1 && M B2
        backpatch(B1.truelist, M.instr)
        B.truelist = B2.truelist
        B.falselist = merge(B1.falselist, B2.falselist)
    */
    unsigned int M = current_function_->get_next_instruction();
    B1->back_patch(current_function_, BackPatchType::True, M);
    BoolExpr B, B2;
    B2 = compare_expr();
    B.true_list = B2.true_list;
    B.false_list = merge(B1->false_list, B2.false_list);
    return B;
  }
  return *B1;
}

// 关系运算表达式
BoolExpr Parser::compare_expr() {
  BoolExpr expr;
  Var lval = alo_expr();
  return compare_tail(lval, &expr);
}
BoolExpr Parser::compare_tail(const Var& lval, BoolExpr* expr) {
  if (COMPARE_FIRST) {
    Operator op = op_compare();
    move_token();
    Var rval = alo_expr();
    debug("a compare expr occur\n");
    // 生成关系运算表达式
    InterInstruction if_jump = InterInstruction::gen_if_jump(
        op, var_to_address(lval), var_to_address(rval));
    add_instruction(&if_jump);
    InterInstruction jump = InterInstruction::gen_jump();
    add_instruction(&jump);
    expr->true_list.push_back(if_jump.index);
    expr->false_list.push_back(jump.index);
  }
  return *expr;
}

// <localdef>-><type> <defdata> <deflist>
void Parser::localdef() {
  Type t = type();
  move_token();
  defdata(t);
  // move_token();
  deflist();
  if (FIRST(Tag::SEMICOLON)) {
    move_token();
  } else {
    recovery();
  }
}

/*
    <expr>-><item> <exprtail> 对应代码 "a + b","a = 10"
    此处因为expr和assign混用，导致一系列问题，需要重构
*/
Var Parser::expr() { return assign_expr(); }

void Parser::operator_() {}

void Parser::operand() {}

Operator Parser::op_low() {
  if (FIRST(Tag::ADD) OR(Tag::SUB)) {
    switch (token_look_.tag()) {
      case Tag::ADD:
        return Operator::OP_ADD;
      case Tag::SUB:
        return Operator::OP_SUB;
    }
  }
  recovery();
  return Operator::OP_INVALID;
}

Operator Parser::op_compare() {
  if (COMPARE_FIRST) {
    switch (token_look_.tag()) {
      case Tag::LESS:
        return Operator::OP_LESS;
      case Tag::LESS_EQUAL:
        return Operator::OP_LESS_EQUAL;
      case Tag::EQUAL:
        return Operator::OP_EQUAL;
      case Tag::NOT_EQUAL:
        return Operator::OP_NOT_EQUAL;
      case Tag::GREATER:
        return Operator::OP_GREATER;
      case Tag::GREATER_EQUAL:
        return Operator::OP_GREATER_EQUAL;
    }
  }
  recovery();
  return Operator::OP_INVALID;
}

/*
算术表达式
*/
Var Parser::alo_expr() {
  Var var = item();
  return alo_tail(var);
}

/*
    加减运算的尾部
    arg1 表示运算符左边的变量
    <alo_tail> -> {+|-} <item> <alo_tail>  code: + a + b;
*/
Var Parser::alo_tail(const Var& arg1) {
  assert(current_function_ != nullptr);
  if (FIRST(Tag::ADD) OR(Tag::SUB)) {
    Operator op = op_low();
    move_token();
    Var arg2 = item();
    Var temp = current_function_->gen_temp_var(arg1.type);
    add_instruction(new InterInstruction(
        var_to_address(temp), op, var_to_address(arg1), var_to_address(arg2)));
    return alo_tail(temp);
  }
  return arg1;
}

bool Parser::match(const Tag& tag) {
  if (token_look_.tag() == tag) {
    move_token();
    return true;
  } else {
    return false;
  }
}
unsigned int Parser::para_transit() {
  unsigned int length = 0;
  Var var = expr();
  // 生成一个表达式的指令，再生成一个保存到参数列表的指令，这里传的是形参
  // 保存到参数列表在汇编中的实现是将表达式结果压栈（无论该变量之前是否已经保存在栈中）。
  // 因为通用的方式是将参数放在调用函数栈帧的底部
  std::cout << "parse one param" << std::endl;
  if (match(Tag::COMMA)) {
    length = para_transit();
  }
  add_instruction(new InterInstruction(Address(), Operator::OP_PUSH_PARAM,
                                       var_to_address(var), Address()));
  length += get_type_size(var.type);
  return length;
}
// 对应代码 10 * a
Var Parser::item() {
  Var var = factor();
  move_token();
  var = itemtail(var);
  return var;
}

/*
   乘除运算尾部
   <itemtail>-><op_high> <factor> <itemtail>  对应代码 * b * c
*/
Var Parser::itemtail(const Var& arg1) {
  assert(current_function_ != nullptr);
  if (FIRST(Tag::MULTIPLY) OR(Tag::DIVIDE)) {
    Operator op = op_high();
    move_token();
    Var arg2 = factor();
    Var result = current_function_->gen_temp_var(arg1.type);
    add_instruction(new InterInstruction(var_to_address(result), op,
                                         var_to_address(arg1),
                                         var_to_address(arg2)));
    move_token();
    return itemtail(result);
  }
  return arg1;
}

Operator Parser::op_high() {
  if (FIRST(Tag::MULTIPLY) OR(Tag::DIVIDE)) {
    switch (token_look_.tag()) {
      case Tag::MULTIPLY:
        return Operator::OP_MUL;
      case Tag::DIVIDE:
        return Operator::OP_DIV;
    }
  }
  recovery();
  return Operator::OP_INVALID;
}

/* <factor> -> val factor()会多读一个token */
Var Parser::factor() { return val(); }

/* <val> -> elem */
Var Parser::val() { return elem(); }

/* <elem>-> ID |ID()| (expr) | literal   elem() 会多读一个token*/
Var Parser::elem() {
  Var var;
  if (FIRST(Tag::IDENTIFIER)) {
    // 从符号表中读取变量
    std::string name = token_look_.get_name();
    move_token();
    var = idexpr(name);

  } else if (FIRST(Tag::LPAREN)) {
    move_token();
    var = expr();
    move_token();
    if (!(FIRST(Tag::RPAREN))) {
      recovery();
    }
  } else {
    var = literal();
  }
  return var;
}

Var Parser::literal() {
  if (FIRST(Tag::LT_NUMBER) OR(Tag::LT_CHAR) OR(Tag::LT_STRING)) {
    Type type = tag_to_type(token_look_.tag());

    if (type == Type::UNKNOWN) {
      error("can not parse the type of literal:");
      recovery();
    } else {
      Var var = Var::create_number(token_look_.get_name());
      return var;
    }
  } else {
    recovery();
  }
  return Var();
}

void Parser::recovery() {
  std::string error_msg = "recovery wrong token ";
  error_msg += token_look_.get_name();
  error(error_msg.c_str());
}

void Parser::move_token() {
  if (index == token_stack_.size() - 1) {
    do {
      token_look_ = lexer_.tokenize();
    } while (token_look_.tag() == Tag::COMMENT);
    token_stack_.push_back(token_look_);
    index++;
  } else {
    token_look_ = token_stack_[++index];
  }
}

/*
    id尾部，可能为函数调用或者是变量定义
    <idtail>-><varrdef> <deflist> | LPAREN <para> RPAREN <funtail>
*/
Var Parser::idtail(const Type& type, std::string name) {
  Var var;
  if (match(Tag::LPAREN)) {
    std::vector<Var> params;
    para(&params);
    if (!(FIRST(Tag::RPAREN))) {
      recovery();
    }
    move_token();
    funtail(type, name, params);
  } else {
    Var result = Var::create_id(name, type);
    var = varrdef(result);
    move_token();
    deflist();
    put_variable(var);
  }
  return var;
}

void Parser::funtail(const Type& type, std::string fun_name,
                     const std::vector<Var>& params) {
  if (FIRST(Tag::SEMICOLON)) {
    std::cout << "function declaration occur name:" << fun_name << std::endl;
    Function func = Function(fun_name, true, type, std::vector<Var>());
    functions_.push_back(func);
    put_function(func);
  } else if (FIRST(Tag::LBRACES)) {
    std::cout << "function definition occur name:" << fun_name << std::endl;
    Function func = Function(fun_name, false, type, params);
    functions_.push_back(func);
    put_function(func);
    current_function_ = &functions_.back();
    block();
    current_function_ = nullptr;
  }
}

/*
    id的表达式
*/
Var Parser::idexpr(std::string name) {
  assert(current_function_ != nullptr);

  Var var;
  if (match(Tag::LPAREN)) {
    // 处理传参
    unsigned int param_length = para_transit();
    if (FIRST(Tag::RPAREN) == false) {
      recovery();
    }

    // 函数调用
    Function fun = get_function(name);
    if (!fun.name.empty()) {
      // 从右往左压参数，可以支持不定长参数，所以不需要传参数个数
      std::cout << "function call occur : " << name << "()" << std::endl;
      Address addr_result = Address();
      if (fun.return_type != Type::VOID && fun.return_type != Type::UNKNOWN) {
        var = current_function_->gen_temp_var(fun.return_type);
        addr_result = var_to_address(var);
      }
      add_instruction(new InterInstruction(
          addr_result, Operator::OP_CALL, Address{LITERAL_STRING, name},
          Address{LITERAL_NUMBER, std::to_string(param_length)}));
      // if (param_length > 0) {
      //  add_instruction(new InterInstruction(Address(),
      //  Operator::OP_POP_PARAM,
      //                                       Address{LITERAL_NUMBER,
      //                                       std::to_string(param_length)},
      //                                       Address()));
      //}
    }
  } else {
    var = current_function_->get_variable(name);
    back_token();
  }
  return var;
}

void Parser::put_variable(Var var) {
  if (var.tag != Tag::IDENTIFIER) {
    return;
  }
  assert(current_function_ != nullptr);
  current_function_->put_variable(var);
}

instr_number Parser::add_instruction(InterInstruction* instrunction) {
  assert(current_function_ != nullptr);
  return current_function_->add_instruction(instrunction);
}

/*
 * 向后移动一个token
 */
void Parser::back_token() {
  if (index > 0) {
    token_look_ = token_stack_[--index];
  }
}

Function Parser::get_function(std::string name) {
  if (function_table_.find(name) != function_table_.end()) {
    return function_table_[name];
  }
  return Function();
}

void Parser::put_function(Function fun) { function_table_[fun.name] = fun; }

std::vector<Function> Parser::get_functions() { return functions_; }

std::vector<instr_number> Parser::merge(std::vector<instr_number> list1,
                                        std::vector<instr_number> list2) {
  std::vector<instr_number> result = std::vector<instr_number>(list1);
  result.insert(result.end(), list2.begin(), list2.end());
  return result;
}
