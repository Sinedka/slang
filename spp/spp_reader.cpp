#include "chartype.h"
#include "spplib.h"
#include "system.h"
#include <format>

#define IF_NEXT_IS(CHAR, THEN_TYPE, ELSE_TYPE)                                 \
  {                                                                            \
    result->type = ELSE_TYPE;                                                  \
    if (*buffer->cur == CHAR)                                                  \
      buffer->cur++, result->type = THEN_TYPE;                                 \
  }

void spp_reader::lex_identifier(spp_string &str) {
  char *cur = this->buffer->cur;

  while (ISIDNUM(*cur))
    cur++;

  str.add(spp_string(this->buffer->cur, cur));

  this->buffer->cur = cur;
}

void spp_reader::lex_string(spp_string &str, sl_ttype &type) {
  char terminator;
  char *cur = this->buffer->cur;

  terminator = *cur++;

  if (terminator == '"')
    type = SPP_STRING;
  else if (terminator == '\'')
    type = SPP_CHAR;
  else
    terminator = '>', type = SPP_HEADER_NAME;

  for (;;) {
    char c = *cur++;

    if (c == '\\') {
      cur++;
    } else if (c == terminator) {
      break;
    } else if (c == '\n') {
      cur--;
      type = SPP_OTHER;
      break;
    }
  }

  if (type == SPP_OTHER)
    new_spp_error(WARNING,
                  std::format("missing terminating %c character", terminator));

  str.add(spp_string(this->buffer->cur, cur));

  buffer->cur = cur;

  if (ISIDNUM(*cur) && terminator != '>')
    lex_identifier(str);
}

void spp_reader::lex_number(spp_string &str) {
  char *cur = this->buffer->cur;

  while (ISIDNUM(*cur) || *cur == '.')
    cur++;

  str.add(spp_string(this->buffer->cur, cur));

  buffer->cur = cur;

  if (ISIDNUM(*cur))
    lex_identifier(str);
}

// just return a token
spp_token *spp_reader::_spp_lex_direct() {
  spp_token *result = cur_token;
  result->val.str = spp_string();
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
    result->type = SPP_EOF;
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
    result->type = SPP_NUMBER;
    buffer->cur--;
    spp_string t = spp_string();
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
    result->type = SPP_NAME;
    spp_string t = spp_string();
    lex_identifier(t);
    result->val.str = t;
    break;
  }
  case '\'':
  case '"': {
    buffer->cur--;
    result->val.str = spp_string();
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

    result->type = SPP_LESS;

    if (*buffer->cur == '=') {
      buffer->cur++, result->type = SPP_LESS_EQ;
    } else if (*buffer->cur == '<') {
      buffer->cur++;
      if (*buffer->cur == '=')
        buffer->cur++, result->type = SPP_LSHIFT_EQ;
      else
        result->type = SPP_LSHIFT;
    }
    break;
  }

  case '>':
    result->type = SPP_GREATER;
    if (*buffer->cur == '=') {
      buffer->cur++, result->type = SPP_GREATER_EQ;
    } else if (*buffer->cur == '>') {
      buffer->cur++;
      if (*buffer->cur == '=')
        buffer->cur++, result->type = SPP_RSHIFT_EQ;
      else
        result->type = SPP_RSHIFT;
    }
    break;
  case '%':
    result->type = SPP_MOD;
    if (*buffer->cur == '=')
      buffer->cur++, result->type = SPP_MOD_EQ;
    break;
  case '.':
    result->type = SPP_DOT;
    break;
  case '+':
    result->type = SPP_PLUS;
    if (*buffer->cur == '+')
      buffer->cur++, result->type = SPP_PLUS_PLUS;
    else if (*buffer->cur == '=')
      buffer->cur++, result->type = SPP_PLUS_EQ;
    break;
  case '-':
    result->type = SPP_MINUS;
    if (*buffer->cur == '>') {
      buffer->cur++;
      result->type = SPP_DEREF;
    } else if (*buffer->cur == '-')
      buffer->cur++, result->type = SPP_MINUS_MINUS;
    else if (*buffer->cur == '=')
      buffer->cur++, result->type = SPP_MINUS_EQ;
    break;
  case '&':
    result->type = SPP_AND;
    if (*buffer->cur == '&')
      buffer->cur++, result->type = SPP_AND_AND;
    else if (*buffer->cur == '=')
      buffer->cur++, result->type = SPP_AND_EQ;
    break;
  case '|':
    result->type = SPP_OR;
    if (*buffer->cur == '|')
      buffer->cur++, result->type = SPP_OR_OR;
    else if (*buffer->cur == '=')
      buffer->cur++, result->type = SPP_OR_EQ;
    break;
  case ':':
    result->type = SPP_COLON;
    if (*buffer->cur == ':') {
      buffer->cur++, result->type = SPP_SCOPE;
    }
    break;

  case '*':
    IF_NEXT_IS('=', SPP_MULT_EQ, SPP_MULT);
    break;
  case '=':
    IF_NEXT_IS('=', SPP_EQ_EQ, SPP_EQ);
    break;
  case '!':
    IF_NEXT_IS('=', SPP_NOT_EQ, SPP_NOT);
    break;
  case '^':
    IF_NEXT_IS('=', SPP_XOR_EQ, SPP_XOR);
    break;
  case '#':
    IF_NEXT_IS('#', SPP_PASTE, SPP_HASH);
    break;
  case '?':
    result->type = SPP_QUERY;
    break;
  case '~':
    result->type = SPP_COMPL;
    break;
  case ',':
    result->type = SPP_COMMA;
    break;
  case '(':
    result->type = SPP_OPEN_PAREN;
    break;
  case ')':
    result->type = SPP_CLOSE_PAREN;
    break;
  case '[':
    result->type = SPP_OPEN_SQUARE;
    break;
  case ']':
    result->type = SPP_CLOSE_SQUARE;
    break;
  case '{':
    result->type = SPP_OPEN_BRACE;
    break;
  case '}':
    result->type = SPP_CLOSE_BRACE;
    break;
  case ';':
    result->type = SPP_SEMICOLON;
    break;

  default: {
    new_spp_error(ERROR, "invalid character");
  }
  }

  result->src_loc = line;
  return result;
}

bool spp_reader::get_fresh_line() {
  while (!buffer->get_fresh_line()) {
    spp_buffer *prev = buffer->get_prev();
    if (prev == nullptr) {
      return false;
    } else {
      buffer->~spp_buffer();
      buffer = prev;
    }
  }
  return true;
}

// call lex direct
spp_token *spp_reader::get_token() {
  if (buffer->cur && *buffer->cur != '\0') {
    if (state.macro_expansion) {
      // TODO: expand macro
    } else {
      spp_token *result = _spp_lex_direct();
      while (result->type == SPP_EOF) {
        if(!get_fresh_line()) break;
        result = _spp_lex_direct();
      }
      result->print();

      return result;
    }
  } else {
    cur_token->type = SPP_EOF;
    cur_token->src_loc = line;
    cur_token->flags = 0;
  }
  // cur_token->print();
  return cur_token;
}

spp_reader::spp_reader(char *filename) {
  buffer = new spp_buffer(filename);
  cur_token = new spp_token();
  cur_token->src_loc=0;
  get_fresh_line();
}

void spp_reader::new_spp_error(spp_error_type type, std::string message) {
  return;
}
