#pragma once
#include "Lexer.h"
enum class Type { UNKNOWN, INT, CHAR, STRING, POINTER, CLASS, VOID };

Type tag_to_type(Tag tag);
