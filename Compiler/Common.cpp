#include "Common.h"

#include <map>
#include <string>

#include "Lexer.h"
#include "Utils.h"
std::map<std::string, Tag> keyword_table;

void init_keyword_table() {
  keyword_table["int"] = Tag::KW_INT;
  keyword_table["char"] = Tag::KW_CHAR;
  keyword_table["string"] = Tag::KW_STRING;
  keyword_table["if"] = Tag::KW_IF;
  keyword_table["else"] = Tag::KW_ELSE;
  keyword_table["do"] = Tag::KW_DO;
  keyword_table["while"] = Tag::KW_WHILE;
  keyword_table["return"] = Tag::KW_RETURN;
  keyword_table["void"] = Tag::KW_VOID;
  keyword_table["for"] = Tag::KW_FOR;
}

Tag get_keyword_tag(std::string key) {
  if (keyword_table.find(key) != keyword_table.end()) {
    return keyword_table[key];
  }
  return Tag::UNKNOWN;
}

void print_error(const char* message) { error(message); }
