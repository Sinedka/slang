#ifndef SLLIB_H
#define SLLIB_H

#include "line_maps.h"
#include "symtab.h"
#include "ttype_table.h"
#include <cstring>
#include <string>

#define OP(e, s) CPP_##e,
#define TK(e, s) CPP_##e,

enum sl_ttype {
  TTYPE_TABLE N_TTYPES,

  /* A token type for keywords, as opposed to ordinary identifiers.  */
  CPP_KEYWORD,

  /* Positions in the table.  */
  CPP_LAST_EQ = CPP_LSHIFT,
  CPP_FIRST_DIGRAPH = CPP_HASH,
  CPP_LAST_PUNCTUATOR = CPP_SCOPE,
  CPP_LAST_SL_OP = CPP_LESS_EQ
};

#undef OP
#undef TK

struct cpp_hashnode;
struct cpp_reader;

class cpp_string;

struct cpp_macro_arg {
  /* Argument number.  */
  unsigned int arg_no;
  /* The original spelling of the macro argument token.  */
  cpp_hashnode *spelling;
};

class cpp_identifier;

class cpp_file {
public:
  std::string filename; // Имя файла
  char *content;        // Содержимое файла в ASCII
  size_t length;        // Длина содержимого

  cpp_file(const std::string &fname);

  ~cpp_file();

  cpp_file(const cpp_file &) = delete;
  cpp_file &operator=(const cpp_file &) = delete;
};

class cpp_string {
    unsigned int len;
    char *text = nullptr;

public:
    cpp_string(char *base, unsigned int size);
    cpp_string(char *base, char *end);
    cpp_string();
    cpp_string(const cpp_string &other);         // копирующий конструктор
    cpp_string(cpp_string &&other) noexcept;     // перемещающий конструктор

    cpp_string &operator=(const cpp_string &other); // копирующее присваивание
    cpp_string &operator=(cpp_string &&other) noexcept; // перемещающее присваивание

    void add(cpp_string str);
    unsigned int get_hash();
    void print();

    ~cpp_string();
};

/* Flags for the cpp_token structure.  */
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
  PURE_ZERO                 /* Set on CPP_NAME tokens whose expansion          \
                               shouldn't start with CPP_DOT or CPP_COLON       \
                               after optional CPP_PADDING.  */
#define SP_DIGRAPH (1 << 8) /* # or ## token was a digraph.  */
#define SP_PREV_WHITE                                                          \
  (1 << 9)                  /* If whitespace before a ##                       \
                               operator, or before this token                  \
                               after a # operator.  */
#define NO_EXPAND (1 << 10) /* Do not macro-expand this token.  */
#define PRAGMA_OP (1 << 11) /* _Pragma token.  */
#define BOL (1 << 12)       /* Token at beginning of line.  */

struct cpp_token;

/* Different flavors of hash node.  */
enum node_type {
  NT_VOID = 0,      /* empty node idk */
  NT_MACRO_ARG,     /* A macro arg.  */
  NT_USER_MACRO,    /* A user macro.  */
  NT_BUILTIN_MACRO, /* A builtin macro.  */
};

/* Different flavors of builtin macro.  _Pragma is an operator, but we
   handle it with the builtin code for efficiency reasons.  */
enum cpp_builtin_type {
  BT_SPECLINE = 0,  /* `__LINE__' */
  BT_DATE,          /* `__DATE__' */
  BT_FILE,          /* `__FILE__' */
  BT_FILE_NAME,     /* `__FILE_NAME__' */
  BT_BASE_FILE,     /* `__BASE_FILE__' */
  BT_INCLUDE_LEVEL, /* `__INCLUDE_LEVEL__' */
  BT_TIME,          /* `__TIME__' */
};

struct cpp_macro {
  /* Parameters, if any. */
  cpp_hashnode **params;

  /* Definition line number.  */
  location_t line;

  unsigned int count;

  /* Number of parameters.  */
  unsigned short paramc;

  /* array of replacement tokens */
  cpp_token *tokens;
};

union _cpp_hashnode_value {
  /* Macro (maybe NULL) */
  cpp_macro *macro;
  /* Code for a builtin macro.  */
  enum cpp_builtin_type builtin;
  /* Macro argument index.  */
  unsigned int arg_index;
};

// base indentifier info
struct ht_identifier {
  cpp_string str;
  unsigned int hash_value;
};

struct cpp_hashnode {
  struct ht_identifier ident;
  unsigned int is_directive : 1;
  unsigned int directive_index : 7; /* If is_directive,
                                       then index into directive table.
                                       Otherwise, a NODE_OPERATOR.  */
  node_type type : 2;               /* CPP node type.  */

  union _cpp_hashnode_value value;
};

class cpp_identifier {
  cpp_hashnode *node;

public:
  cpp_identifier();
  cpp_identifier(cpp_string str);
  void change_type(node_type type);
};

struct cpp_token {
  /* Location of first char of token, together with range of full token.  */
  location_t src_loc;

  sl_ttype type;        /* token type */
  unsigned short flags; /* flags - see above */

  union cpp_token_u {
    /* An identifier.  */
    struct cpp_identifier node;

    /* A string, or number.  */
    struct cpp_string str;

    /* Argument no. for a CPP_MACRO_ARG.  */
    struct cpp_macro_arg macro_arg;

    // other
    void *other;

    cpp_token_u();
    ~cpp_token_u();

  } val;

  cpp_token();
};

/* Represents the contents of a file cpplib has read in.  */
class cpp_buffer {
public:
  char *cur; /* Current location.  */

  cpp_buffer(char *filename);
  cpp_buffer(cpp_buffer *prev, char *filename);
  ~cpp_buffer();
  bool get_fresh_line();

  cpp_buffer *get_prev();

private:
  cpp_buffer *prev;

  char *file_cur;

  char *line_begin;

  /* Pointer into the file table; non-NULL if this is a file buffer.
     Used for include_next and to record control macros.  */
  cpp_file *file;
};

struct lexer_state {
  bool macro_expansion : 1;
  bool in_include : 1;
};

struct macro_context {
  /* The node of the macro we are referring to.  */
  cpp_hashnode *macro_node;
  /* This buffer contains an array of virtual locations.  The virtual
     location at index 0 is the virtual location of the token at index
     0 in the current instance of cpp_context; similarly for all the
     other virtual locations.  */
  location_t *virt_locs;
  /* This is a pointer to the current virtual location.  This is used
     to iterate over the virtual locations while we iterate over the
     tokens they belong to.  */
  location_t *cur_virt_loc;
};

/* The kind of tokens carried by a cpp_context.  */
enum context_tokens_kind {
  /* This is the value of cpp_context::tokens_kind if u.iso.first
     contains an instance of cpp_token **.  */
  TOKENS_KIND_INDIRECT,
  /* This is the value of cpp_context::tokens_kind if u.iso.first
     contains an instance of cpp_token *.  */
  TOKENS_KIND_DIRECT,
  /* This is the value of cpp_context::tokens_kind when the token
     context contains tokens resulting from macro expansion.  In that
     case struct cpp_context::macro points to an instance of struct
     macro_context.  This is used only when the
     -ftrack-macro-expansion flag is on.  */
  TOKENS_KIND_EXTENDED
};

enum cpp_error_type { WARNING, ERROR };

class cpp_error {
private:
  std::string message;
  cpp_error_type type : 8;
  location_t pos;

public:
  cpp_error(cpp_error_type er_type, std::string mes, location_t pos);
  cpp_error();
};

/* A cpp_reader encapsulates the "state" of a pre-processor run.
   Applying cpp_get_token repeatedly yields a stream of pre-processor
   tokens.  Usually, there is only one cpp_reader object active.  */
class cpp_reader {
private:
  /* Top of buffer stack.  */
  cpp_buffer *buffer;

  /* Lexer state.  */
  lexer_state state;

  location_t line;

  /* Lexing.  */
  cpp_token *cur_token;

  /* A token demarking macro arguments.  */
  cpp_token endarg;

  /* Identifier hash table.  */
  struct ht *hash_table;

  cpp_token *_cpp_lex_direct();
  void lex_number(cpp_string &str);
  void lex_identifier(cpp_string &str);
  void lex_string(cpp_string &str, sl_ttype &type);

  // expand directive if needed
  // false if nothing left
  bool get_fresh_line();

  bool read_line();
  cpp_token expand_macro();
  void new_cpp_error(cpp_error_type type, std::string message);

public:
  cpp_token *get_token();
  void pop_buffer();
};

#endif
