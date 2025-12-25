#include "chartype.h"
#include "spplib.h"
#include "system.h"
#include <format>
#include <iostream>

#define IF_NEXT_IS(CHAR, THEN_TYPE, ELSE_TYPE)                                 \
  {                                                                            \
    result->type = ELSE_TYPE;                                                  \
    if (*buffer->cur == CHAR)                                                  \
      buffer->cur++, result->type = THEN_TYPE;                                 \
  }

void cpp_reader::lex_identifier(cpp_string &str) {
  char *cur = this->buffer->cur;

  while (ISIDNUM(*cur))
    cur++;

  str.add(cpp_string(this->buffer->cur, cur));

  this->buffer->cur = cur;
}

void cpp_reader::lex_string(cpp_string &str, sl_ttype &type) {
  char terminator;
  char *cur = this->buffer->cur;

  terminator = *cur++;

  if (terminator == '"')
    type = CPP_STRING;
  else if (terminator == '\'')
    type = CPP_CHAR;
  else
    terminator = '>', type = CPP_HEADER_NAME;

  for (;;) {
    char c = *cur++;

    if (c == '\\') {
      cur++;
    } else if (c == terminator) {
      break;
    } else if (c == '\n') {
      cur--;
      type = CPP_OTHER;
      break;
    }
  }

  if (type == CPP_OTHER)
    new_cpp_error(WARNING,
                  std::format("missing terminating %c character", terminator));

  str.add(cpp_string(this->buffer->cur, cur));

  buffer->cur = cur;

  if (ISIDNUM(*cur) && terminator != '>')
    lex_identifier(str);
}

void cpp_reader::lex_number(cpp_string &str) {
  char *cur = this->buffer->cur;

  while (ISIDNUM(*cur) || *cur == '.')
    cur++;

  str.add(cpp_string(this->buffer->cur, cur));

  buffer->cur = cur;

  if (ISIDNUM(*cur))
    lex_identifier(str);
}

// just return a token
cpp_token *cpp_reader::_cpp_lex_direct() {
  cpp_token *result = cur_token;
  result->val.str = cpp_string();
  char c = 0;
  result->flags = 0;

whitespace:
  c = *buffer->cur++;
  switch (c) {
  case ' ':
  case '\t':
    goto whitespace;
  case '\n': {
    line++;
    result->type = CPP_EOF;
    break;
  }
  case '0':
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
  case '9': {
    result->type = CPP_NUMBER;
    buffer->cur--;
    cpp_string t = cpp_string();
    lex_number(t);
    result->val.str = t;
    break;
  }
  case '_':
  case 'a':
  case 'b':
  case 'c':
  case 'd':
  case 'e':
  case 'f':
  case 'g':
  case 'h':
  case 'i':
  case 'j':
  case 'k':
  case 'l':
  case 'm':
  case 'n':
  case 'o':
  case 'p':
  case 'q':
  case 'r':
  case 's':
  case 't':
  case 'u':
  case 'v':
  case 'w':
  case 'x':
  case 'y':
  case 'z':
  case 'A':
  case 'B':
  case 'C':
  case 'D':
  case 'E':
  case 'F':
  case 'G':
  case 'H':
  case 'I':
  case 'J':
  case 'K':
  case 'L':
  case 'M':
  case 'N':
  case 'O':
  case 'P':
  case 'Q':
  case 'R':
  case 'S':
  case 'T':
  case 'U':
  case 'V':
  case 'W':
  case 'X':
  case 'Y':
  case 'Z': {
    buffer->cur--;
    result->type = CPP_NAME;
    cpp_string t = cpp_string();
    lex_identifier(t);
    result->val.node = cpp_identifier(t);
    break;
  }
  case '\'':
  case '"': {
    buffer->cur--;
    result->val.str = cpp_string();
    this->lex_string(result->val.str, result->type);

    break;
  }
  case '/':
    // TODO: comment
    goto whitespace;
  case '<': {
    // if (this->state.in_include) {
    //   lex_string(result->val.str, result->type);
    // }

    result->type = CPP_LESS;

    if (*buffer->cur == '=') {
      buffer->cur++, result->type = CPP_LESS_EQ;
    } else if (*buffer->cur == '<') {
      buffer->cur++;
      if (*buffer->cur == '=')
        buffer->cur++, result->type = CPP_LSHIFT_EQ;
      else
        result->type = CPP_LSHIFT;
    }
    break;
  }

  case '>':
    result->type = CPP_GREATER;
    if (*buffer->cur == '=') {
      buffer->cur++, result->type = CPP_GREATER_EQ;
    } else if (*buffer->cur == '>') {
      buffer->cur++;
      if (*buffer->cur == '=')
        buffer->cur++, result->type = CPP_RSHIFT_EQ;
      else
        result->type = CPP_RSHIFT;
    }
    break;
  case '%':
    result->type = CPP_MOD;
    if (*buffer->cur == '=')
      buffer->cur++, result->type = CPP_MOD_EQ;
    break;
  case '.':
    result->type = CPP_DOT;
    break;
  case '+':
    result->type = CPP_PLUS;
    if (*buffer->cur == '+')
      buffer->cur++, result->type = CPP_PLUS_PLUS;
    else if (*buffer->cur == '=')
      buffer->cur++, result->type = CPP_PLUS_EQ;
    break;
  case '-':
    result->type = CPP_MINUS;
    if (*buffer->cur == '>') {
      buffer->cur++;
      result->type = CPP_DEREF;
    } else if (*buffer->cur == '-')
      buffer->cur++, result->type = CPP_MINUS_MINUS;
    else if (*buffer->cur == '=')
      buffer->cur++, result->type = CPP_MINUS_EQ;
    break;
  case '&':
    result->type = CPP_AND;
    if (*buffer->cur == '&')
      buffer->cur++, result->type = CPP_AND_AND;
    else if (*buffer->cur == '=')
      buffer->cur++, result->type = CPP_AND_EQ;
    break;
  case '|':
    result->type = CPP_OR;
    if (*buffer->cur == '|')
      buffer->cur++, result->type = CPP_OR_OR;
    else if (*buffer->cur == '=')
      buffer->cur++, result->type = CPP_OR_EQ;
    break;
  case ':':
    result->type = CPP_COLON;
    if (*buffer->cur == ':') {
      buffer->cur++, result->type = CPP_SCOPE;
    }
    break;

  case '*':
    IF_NEXT_IS('=', CPP_MULT_EQ, CPP_MULT);
    break;
  case '=':
    IF_NEXT_IS('=', CPP_EQ_EQ, CPP_EQ);
    break;
  case '!':
    IF_NEXT_IS('=', CPP_NOT_EQ, CPP_NOT);
    break;
  case '^':
    IF_NEXT_IS('=', CPP_XOR_EQ, CPP_XOR);
    break;
  case '#':
    IF_NEXT_IS('#', CPP_PASTE, CPP_HASH);
    break;
  case '?':
    result->type = CPP_QUERY;
    break;
  case '~':
    result->type = CPP_COMPL;
    break;
  case ',':
    result->type = CPP_COMMA;
    break;
  case '(':
    result->type = CPP_OPEN_PAREN;
    break;
  case ')':
    result->type = CPP_CLOSE_PAREN;
    break;
  case '[':
    result->type = CPP_OPEN_SQUARE;
    break;
  case ']':
    result->type = CPP_CLOSE_SQUARE;
    break;
  case '{':
    result->type = CPP_OPEN_BRACE;
    break;
  case '}':
    result->type = CPP_CLOSE_BRACE;
    break;
  case ';':
    result->type = CPP_SEMICOLON;
    break;

  default: {
    new_cpp_error(ERROR, "invalid character");
  }
  }

  result->src_loc = line;
  return result;
}

bool cpp_reader::get_fresh_line() {
  while (!buffer->get_fresh_line()) {
    cpp_buffer *prev = buffer->get_prev();
    if (prev == nullptr) {
      return false;
    } else {
      buffer->~cpp_buffer();
      buffer = prev;
    }
  }
  return true;
}

// call lex direct
cpp_token *cpp_reader::get_token() {
  if (buffer->cur && *buffer->cur != '\0') {
    if (state.macro_expansion) {
      // TODO: expand macro
    } else {
      cpp_token *result = _cpp_lex_direct();
      while (result->type == CPP_EOF) {
        if(!get_fresh_line()) break;
        result = _cpp_lex_direct();
      }

      return result;
    }
  } else {
    cur_token->type = CPP_EOF;
    cur_token->src_loc = line;
    cur_token->flags = 0;
  }
  return cur_token;
}

cpp_reader::cpp_reader(char *filename) {
  buffer = new cpp_buffer(filename);
  cur_token = new cpp_token();
  get_fresh_line();
}

void cpp_reader::new_cpp_error(cpp_error_type type, std::string message) {
  return;
}
