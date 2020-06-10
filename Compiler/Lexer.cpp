#include "Lexer.h"

#include <iostream>

#include "Common.h"
#include "Utils.h"

Token Token::unknown_token = Token();

Lexer::Lexer() { pcurrent_token_ = nullptr; }

void Lexer::scan(const char* file_name) {
  if (file_string_.empty() && file_name != nullptr) {
    file_string_ = read_string_from_file(file_name);
  }
  token_string_ = "";
  do {
    tokenize();
  } while (pcurrent_token_->get_tag() != Tag::END_OF_FILE);
}

void Lexer::reset() {
  file_string_ = "";
  scan_pos_ = 0;
}

void Lexer::load_file(const char* file_name) {
  if (file_name != nullptr) {
    file_string_ = read_string_from_file(file_name);
  }
}

const Token& Lexer::tokenize() {
  char current_char = get_char();
  if (is_blank(current_char)) {
    skip_white_space();
    current_char = get_char();
  }
  if (is_digit(current_char)) {
    parse_number(current_char);
  } else if (is_alphabet(current_char)) {
    parse_word(current_char);
  } else if (current_char == '\'') {
    parse_character(current_char);
  } else if (current_char == '\"') {
    parse_string(current_char);
  } else if (current_char == '/') {
    parse_comment(current_char);
  } else if (current_char == '=') {
    token_string_.append(1, current_char);
    current_char = get_char();
    if (current_char == '=') {
      token_string_.append(1, current_char);
      pcurrent_token_ = new Token(Tag::EQUAL, token_string_);
    } else {
      unget_char();
      pcurrent_token_ = new Token(Tag::ASSIGN, token_string_);
    }
    token_string_ = "";
  } else if (current_char == ';') {
    token_string_ = ";";
    pcurrent_token_ = new Token(Tag::SEMICOLON, token_string_);
    token_string_ = "";
  } else if (current_char == '(') {
    token_string_ = "(";
    pcurrent_token_ = new Token(Tag::LPAREN, token_string_);
    token_string_ = "";
  } else if (current_char == ')') {
    token_string_ = ")";
    pcurrent_token_ = new Token(Tag::RPAREN, token_string_);
    token_string_ = "";
  } else if (current_char == '[') {
    token_string_ = "[";
    pcurrent_token_ = new Token(Tag::LBRACKET, token_string_);
    token_string_ = "";
  } else if (current_char == ']') {
    token_string_ = "]";
    pcurrent_token_ = new Token(Tag::RBRACKET, token_string_);
    token_string_ = "";
  } else if (current_char == '{') {
    token_string_ = "{";
    pcurrent_token_ = new Token(Tag::LBRACES, token_string_);
    token_string_ = "";
  } else if (current_char == '}') {
    token_string_ = "}";
    pcurrent_token_ = new Token(Tag::RBRACES, token_string_);
    token_string_ = "";
  } else if (current_char == '+') {
    token_string_ = "+";
    pcurrent_token_ = new Token(Tag::ADD, token_string_);
    token_string_ = "";
  } else if (current_char == '-') {
    token_string_ = "-";
    pcurrent_token_ = new Token(Tag::SUB, token_string_);
    token_string_ = "";
  } else if (current_char == '*') {
    token_string_ = "*";
    pcurrent_token_ = new Token(Tag::MULTIPLY, token_string_);
    token_string_ = "";
  } else if (current_char == '/') {
    token_string_ = "/";
    pcurrent_token_ = new Token(Tag::DIVIDE, token_string_);
    token_string_ = "";
  } else if (current_char == ',') {
    token_string_ = ",";
    pcurrent_token_ = new Token(Tag::COMMA, token_string_);
    token_string_ = "";
  } else if (current_char == '<') {
    token_string_.append(1, current_char);
    current_char = get_char();
    if (current_char == '=') {
      token_string_.append(1, current_char);
      pcurrent_token_ = new Token(Tag::LESS_EQUAL, token_string_);
    } else {
      unget_char();
      pcurrent_token_ = new Token(Tag::LESS, token_string_);
    }
    token_string_ = "";
  } else if (current_char == '>') {
    token_string_.append(1, current_char);
    current_char = get_char();
    if (current_char == '=') {
      token_string_.append(1, current_char);
      pcurrent_token_ = new Token(Tag::GREATER_EQUAL, token_string_);
    } else {
      unget_char();
      pcurrent_token_ = new Token(Tag::GREATER, token_string_);
    }
    token_string_ = "";
  } else if (current_char == '!') {
    token_string_ = "!";
    current_char = get_char();
    if (current_char == '=') {
      token_string_.append(1, current_char);
      pcurrent_token_ = new Token(Tag::NOT_EQUAL, token_string_);
    } else {
      unget_char();
      pcurrent_token_ = new Token(Tag::NOT, token_string_);
    }
    token_string_ = "";

  } else if (current_char == '&') {
    token_string_.append(1, current_char);
    current_char = get_char();
    if (current_char == '&') {
      token_string_.append(1, current_char);
      pcurrent_token_ = new Token(Tag::AND, token_string_);
    } else {
      unget_char();
      pcurrent_token_ = new Token(Tag::BITWISE_AND, token_string_);
    }
    token_string_ = "";
  } else if (current_char == '|') {
    token_string_.append(1, current_char);
    current_char = get_char();
    if (current_char == '|') {
      token_string_.append(1, current_char);
      pcurrent_token_ = new Token(Tag::OR, token_string_);
    } else {
      unget_char();
      pcurrent_token_ = new Token(Tag::BITWISE_OR, token_string_);
    }
    token_string_ = "";
  } else if (current_char == EOF) {
    token_string_ = "end of file";
    pcurrent_token_ = new Token(Tag::END_OF_FILE, token_string_);
    token_string_ = "";
  }
  if (pcurrent_token_ != nullptr) {
    std::cout << "occur token " << pcurrent_token_->to_string() << std::endl;
    return *pcurrent_token_;
  } else {
    return Token::unknown_token;
  }
}

char Lexer::get_char() {
  if (scan_pos_ < file_string_.size()) {
    return file_string_.c_str()[scan_pos_++];
  }
  return EOF;
}

void Lexer::parse_number(char ch) {
  token_string_.append(1, ch);
  do {
    ch = get_char();
    if (is_digit(ch)) {
      token_string_.append(1, ch);
    } else {
      unget_char();
      pcurrent_token_ = new Token(Tag::LT_NUMBER, token_string_);
      break;
    }
  } while (ch != EOF);
  token_string_ = "";
}

void Lexer::parse_word(char ch) {
  token_string_.append(1, ch);
  do {
    ch = get_char();
    if (is_alphabet(ch) || is_digit(ch) || ch == '_') {
      token_string_.append(1, ch);
    } else {
      unget_char();
      Tag tag;
      if ((tag = get_keyword_tag(token_string_)) != Tag::UNKNOWN) {
        pcurrent_token_ = new Token(tag, token_string_);
      } else {
        pcurrent_token_ = new Token(Tag::IDENTIFIER, token_string_);
      }
      break;
    }
  } while (ch != EOF);
  token_string_ = "";
}

void Lexer::parse_character(char ch) {
  token_string_.append(1, ch);
  ch = get_char();
  if (ch == '\\') {
    ch = get_char();
    switch (ch) {
      case 'a':
      case 'b':
      case 'f':
      case 'n':
      case 'r':
      case 't':
      case 'v':
      case '\\':
      case '\'':
      case '\?':
      case '\0':
      case '\"':
        token_string_.append(1, ch);
        break;
      default:
        error(std::string("illegal character after \\").append(1, ch).c_str());
        break;
    }
  } else {
    token_string_.append(1, ch);
  }
  ch = get_char();
  if (ch == '\'') {
    token_string_.append(1, ch);
    pcurrent_token_ = new Token(Tag::LT_CHAR, token_string_);
  } else {
    error(std::string("expected \' but").append(1, ch).c_str());
  }
  token_string_ = "";
}

void Lexer::parse_comment(char ch) {
  token_string_.append(1, ch);
  ch = get_char();
  if (ch == '/') {
    do {
      ch = get_char();
      token_string_.append(1, ch);
    } while (ch != '\n' && ch != EOF);
    token_string_ = token_string_.substr(token_string_.size() - 1);
    pcurrent_token_ = new Token(Tag::COMMENT, token_string_);
  } else if (ch == '*') {
    int state = 0;
    do {
      ch = get_char();
      if (ch == EOF) {
        error("uncomplete comment");
        break;
      } else if (ch == '*') {
        state = '*';
        token_string_.append(1, ch);
        continue;
      }
      if (state == '*' && ch == '/') {
        token_string_.append(1, ch);
        break;
      } else {
        state = 0;
        token_string_.append(1, ch);
      }
    } while (true);
    pcurrent_token_ = new Token(Tag::COMMENT, token_string_);
  } else {
    error("illegal comment character");
  }
  token_string_ = "";
}

void Lexer::skip_white_space() {
  char ch = ' ';
  do {
    ch = get_char();
  } while (is_blank(ch) && ch != EOF);
  if (ch != EOF) {
    unget_char();
  }
}

void Lexer::unget_char() {
  if (scan_pos_ > 0) {
    scan_pos_--;
  }
}

void Lexer::parse_string(char ch) {
  token_string_.append(1, ch);
  int STATE_NORMAL = 0;
  int STATE_ESCAPE = 1;
  int state = STATE_NORMAL;
  do {
    ch = get_char();
    token_string_.append(1, ch);
    if (state == STATE_NORMAL) {
      if (ch == '\"') {
        pcurrent_token_ = new Token(Tag::LT_STRING, token_string_);
        break;
      } else if (ch == '\\') {
        state = STATE_ESCAPE;
      }
    } else if (state == STATE_ESCAPE) {
      switch (ch) {
        case 'a':
        case 'b':
        case 'f':
        case 'n':
        case 'r':
        case 't':
        case 'v':
        case '\\':
        case '\'':
        case '\"':
        case '\?':
        case '\0':
          state = STATE_NORMAL;
          continue;
        default:
          error(
              std::string("illegal character after \\").append(1, ch).c_str());
      }
    }
  } while (true);
}