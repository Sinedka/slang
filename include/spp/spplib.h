#ifndef SLLIB_H
#define SLLIB_H

#include "line_maps.h"
#include "symtab.h"
#include "ttype_table.h"
#include <cstring>
#include <iostream>
#include <string>

#define OP(e, s) SPP_##e,
#define TK(e, s) SPP_##e,

enum sl_ttype {
  TTYPE_TABLE N_TTYPES,

  /* A token type for keywords, as opposed to ordinary identifiers.  */
  SPP_KEYWORD,

  /* Positions in the table.  */
  // SPP_LAST_EQ = SPP_LSHIFT,
  // SPP_FIRST_DIGRAPH = SPP_HASH,
  // SPP_LAST_PUNCTUATOR = SPP_SCOPE,
  // SPP_LAST_SL_OP = SPP_LESS_EQ
};

#undef OP
#undef TK

struct spp_hashnode;
struct spp_reader;

class spp_string;

struct spp_macro_arg {
  /* Argument number.  */
  unsigned int arg_no;
  /* The original spelling of the macro argument token.  */
  spp_hashnode *spelling;
};

class spp_identifier;

class spp_file {
public:
  std::string filename; // Имя файла
  char *content;        // Содержимое файла в ASCII
  size_t length;        // Длина содержимого

  spp_file(const std::string &fname);

  ~spp_file();

  spp_file(const spp_file &) = delete;
  spp_file &operator=(const spp_file &) = delete;
};

class spp_string {

public:
  unsigned int len;
  char *text = nullptr;
  spp_string(char *base, unsigned int size);
  spp_string(char *base, char *end);
  spp_string();
  spp_string(const spp_string &other);     // копирующий конструктор
  spp_string(spp_string &&other) noexcept; // перемещающий конструктор

  spp_string &operator=(const spp_string &other); // копирующее присваивание
  spp_string &
  operator=(spp_string &&other) noexcept; // перемещающее присваивание

  void add(spp_string str);
  unsigned int get_hash();
  void print();

  ~spp_string();
};

/* Flags for the spp_token structure.  */
#define PREV_WHITE (1 << 0)    /* If whitespace before this token.  */
#define DIGRAPH (1 << 1)       /* If it was a digraph.  */
#define STRINGIFY_ARG (1 << 2) /* If macro argument to be stringified.  */
#define PASTE_LEFT (1 << 3)    /* If on LHS of a ## operator.  */
#define NAMED_OP (1 << 4)      /* C++ named operators.  */
#define PREV_FALLTHROUGH                                                       \
  (1 << 5)                   /* On a token preceeded by FALLTHROUGH comment */
#define DECIMAL_INT (1 << 6) /* Decimal integer, set in c-lex.cc.  */
#define PURE_ZERO                                                              \
  (1 << 7) /* Single 0 digit, used by the C++ frontend, set in c-lex.cc.  */
#define COLON_SCOPE PURE_ZERO /* Adjacent colons in C < 23.  */
#define NO_DOT_COLON                                                           \
  PURE_ZERO                 /* Set on SPP_NAME tokens whose expansion          \
                               shouldn't start with SPP_DOT or SPP_COLON       \
                               after optional SPP_PADDING.  */
#define SP_DIGRAPH (1 << 8) /* # or ## token was a digraph.  */
#define SP_PREV_WHITE                                                          \
  (1 << 9)                  /* If whitespace before a ##                       \
                               operator, or before this token                  \
                               after a # operator.  */
#define NO_EXPAND (1 << 10) /* Do not macro-expand this token.  */
#define PRAGMA_OP (1 << 11) /* _Pragma token.  */
#define BOL (1 << 12)       /* Token at beginning of line.  */

struct spp_token;

/* Different flavors of hash node.  */
enum node_type {
  NT_VOID = 0,      /* empty node idk */
  NT_MACRO_ARG,     /* A macro arg.  */
  NT_USER_MACRO,    /* A user macro.  */
  NT_BUILTIN_MACRO, /* A builtin macro.  */
};

/* Different flavors of builtin macro.  _Pragma is an operator, but we
   handle it with the builtin code for efficiency reasons.  */
enum spp_builtin_type {
  BT_SPECLINE = 0,  /* `__LINE__' */
  BT_DATE,          /* `__DATE__' */
  BT_FILE,          /* `__FILE__' */
  BT_FILE_NAME,     /* `__FILE_NAME__' */
  BT_BASE_FILE,     /* `__BASE_FILE__' */
  BT_INCLUDE_LEVEL, /* `__INCLUDE_LEVEL__' */
  BT_TIME,          /* `__TIME__' */
};

struct spp_macro {
  /* Parameters, if any. */
  spp_hashnode **params;

  /* Definition line number.  */
  location_t line;

  unsigned int count;

  /* Number of parameters.  */
  unsigned short paramc;

  /* array of replacement tokens */
  spp_token *tokens;
};

union _spp_hashnode_value {
  /* Macro (maybe NULL) */
  spp_macro *macro;
  /* Code for a builtin macro.  */
  enum spp_builtin_type builtin;
  /* Macro argument index.  */
  unsigned int arg_index;
};

// base indentifier info
struct ht_identifier {
  spp_string str;
  unsigned int hash_value;
};

struct spp_hashnode {
  struct ht_identifier ident;
  unsigned int is_directive : 1;
  unsigned int directive_index : 7; /* If is_directive,
                                       then index into directive table.
                                       Otherwise, a NODE_OPERATOR.  */
  node_type type : 2;               /* SPP node type.  */

  union _spp_hashnode_value value;
};

class spp_identifier {
  spp_hashnode *node;

public:
  spp_identifier();
  spp_identifier(spp_string str);
  void change_type(node_type type);
};

struct spp_token {
  /* Location of first char of token, together with range of full token.  */
  location_t src_loc;

  sl_ttype type;        /* token type */
  unsigned short flags; /* flags - see above */

  union spp_token_u {
    /* An identifier.  */
    // struct spp_identifier node;

    /* A string, or number.  */
    struct spp_string str;

    /* Argument no. for a SPP_MACRO_ARG.  */
    // struct spp_macro_arg macro_arg;

    // other
    void *other1;

    spp_token_u() {
      str = spp_string();
      return;
    };
    ~spp_token_u();

  } val;

  spp_token() { return; };
  void print() {
    std::cout << "token_type: ";
    if (type == 49) {
      std::cout << "SPP_NAME ";
      val.str.print();

    } else if (type == SPP_NUMBER) {
      std::cout << "SPP_NUMBER ";
      val.str.print();
    } else if (type == SPP_CHAR) {
      std::cout << "SPP_CHAR ";
      val.str.print();
    } else if (type == SPP_OTHER) {
      std::cout << "SPP_OTHER ";
      val.str.print();
    } else if (type == SPP_STRING) {
      std::cout << "SPP_STRING ";
      val.str.print();

    } else if (type == SPP_EOF) {
      std::cout << "SPP_EOF; ";

    } else {
      std::cout << "OPERATION ";
      static const char *ttype_to_string[] = {
#define OP(e, s) [SPP_##e] = s,
#define TK(e, s) [SPP_##e] = #e,
          TTYPE_TABLE
#undef OP
#undef TK

      };
      std::cout << ttype_to_string[type] << ' ';
    }
    std::cout << src_loc << "\n";
  }
};

/* Represents the contents of a file spplib has read in.  */
class spp_buffer {
public:
  char *cur; /* Current location.  */

  spp_buffer(char *filename);
  spp_buffer(spp_buffer *prev, char *filename);
  ~spp_buffer();
  bool get_fresh_line();

  spp_buffer *get_prev();

private:
  spp_buffer *prev;

  char *file_cur;

  char *line_begin;

  /* Pointer into the file table; non-NULL if this is a file buffer.
     Used for include_next and to record control macros.  */
  spp_file *file;
};

struct lexer_state {
  bool macro_expansion : 1;
  bool in_include : 1;
};

struct macro_context {
  /* The node of the macro we are referring to.  */
  spp_hashnode *macro_node;
  /* This buffer contains an array of virtual locations.  The virtual
     location at index 0 is the virtual location of the token at index
     0 in the current instance of spp_context; similarly for all the
     other virtual locations.  */
  location_t *virt_locs;
  /* This is a pointer to the current virtual location.  This is used
     to iterate over the virtual locations while we iterate over the
     tokens they belong to.  */
  location_t *cur_virt_loc;
};

/* The kind of tokens carried by a spp_context.  */
enum context_tokens_kind {
  /* This is the value of spp_context::tokens_kind if u.iso.first
     contains an instance of spp_token **.  */
  TOKENS_KIND_INDIRECT,
  /* This is the value of spp_context::tokens_kind if u.iso.first
     contains an instance of spp_token *.  */
  TOKENS_KIND_DIRECT,
  /* This is the value of spp_context::tokens_kind when the token
     context contains tokens resulting from macro expansion.  In that
     case struct spp_context::macro points to an instance of struct
     macro_context.  This is used only when the
     -ftrack-macro-expansion flag is on.  */
  TOKENS_KIND_EXTENDED
};

enum spp_error_type { WARNING, ERROR };

class spp_error {
private:
  std::string message;
  spp_error_type type : 8;
  location_t pos;

public:
  spp_error(spp_error_type er_type, std::string mes, location_t pos);
  spp_error();
};

/* A spp_reader encapsulates the "state" of a pre-processor run.
   Applying spp_get_token repeatedly yields a stream of pre-processor
   tokens.  Usually, there is only one spp_reader object active.  */
class spp_reader {
private:
  /* Top of buffer stack.  */
  spp_buffer *buffer;

  /* Lexer state.  */
  lexer_state state;

  location_t line;

  /* Lexing.  */
  spp_token *cur_token;

  /* Identifier hash table.  */
  struct ht *hash_table;

  spp_token *_spp_lex_direct();
  void lex_number(spp_string &str);
  void lex_identifier(spp_string &str);
  void lex_string(spp_string &str, sl_ttype &type);

  // expand directive if needed
  // false if nothing left
  bool get_fresh_line();

  bool read_line();
  spp_token expand_macro();
  void new_spp_error(spp_error_type type, std::string message);

public:
  spp_token *get_token();
  spp_reader(char *filename);
};

#endif
