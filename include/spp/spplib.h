#ifndef SLLIB_H
#define SLLIB_H

#include "line_maps.h"
#include "ttype_table.h"
#include "libiberty.h"

#define OP(e, s) SL_##e,
#define TK(e, s) SL_##e,

enum sl_ttype {
  TTYPE_TABLE N_TTYPES,

  /* A token type for keywords, as opposed to ordinary identifiers.  */
  SL_KEYWORD,

  /* Positions in the table.  */
  SL_LAST_EQ = SL_LSHIFT,
  SL_FIRST_DIGRAPH = SL_HASH,
  SL_LAST_PUNCTUATOR = SL_SCOPE,
  SL_LAST_SL_OP = SL_LESS_EQ
};

#undef OP
#undef TK

struct cpp_hashnode;

struct cpp_macro_arg {
  /* Argument number.  */
  unsigned int arg_no;
  /* The original spelling of the macro argument token.  */
  cpp_hashnode *spelling;
};

struct cpp_identifier {
  cpp_hashnode *node;
};

struct cpp_string {
  unsigned int len;

  /* TEXT is always null terminated (terminator not included in len) */
  const unsigned char *text;
};

struct cpp_token {

  /* Location of first char of token, together with range of full token.  */
  location_t src_loc;

  sl_ttype type : 8; /* token type */

  union cpp_token_u {
    /* An identifier.  */
    struct cpp_identifier node;

    /* A string, or number.  */
    struct cpp_string str;

    /* Argument no. for a CPP_MACRO_ARG.  */
    struct cpp_macro_arg macro_arg;
  } val;
};

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

  /* Number of tokens in body, or bytes for traditional macros.  */
  // TODO: Do we really need 32 bits here?
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
  const unsigned char *str;
  unsigned int len;
  unsigned int hash_value;
};

struct cpp_hashnode {
  struct ht_identifier ident;
  unsigned int is_directive : 1;
  unsigned int directive_index : 7; /* If is_directive,
                                       then index into directive table.
                                       Otherwise, a NODE_OPERATOR.  */
  node_type type : 2;               /* CPP node type.  */

  // TODO: flags for optimization

  /* 54 bits spare  */

  union _cpp_hashnode_value value;
};

struct cpp_dir {
  /* NULL-terminated singly-linked list.  */
  struct cpp_dir *next;

  /* NAME of the directory, NUL-terminated.  */
  char *name;
  unsigned int len;

  /* One if a system header, two if a system header that has extern
     "C" guards for C++.  */
  unsigned char sysp;

  /* Is this a user-supplied directory? */
  bool user_supplied_p;

  /* The canonicalized NAME as determined by lrealpath.  This field
     is only used by hosts that lack reliable inode numbers.  */
  char *canonical_name;

  /* Mapping of file names for this directory for MS-DOS and related
     platforms.  A NULL-terminated array of (from, to) pairs.  */
  const char **name_map;

  /* Routine to construct pathname, given the search path name and the
     HEADER we are trying to find, return a constructed pathname to
     try and open.  If this is NULL, the constructed pathname is as
     constructed by append_file_to_dir.  */
  char *(*construct)(const char *header, cpp_dir *dir);
};

/* Represents the contents of a file cpplib has read in.  */
struct cpp_buffer {
  const unsigned char *cur;       /* Current location.  */
  const unsigned char *line_base; /* Start of current physical line.  */
  const unsigned char *next_line; /* Start of to-be-cleaned logical line.  */

  const unsigned char *buf;     /* Entire character buffer.  */
  const unsigned char *rlimit;  /* Writable byte at end of file.  */
  const unsigned char *to_free; /* Pointer that should be freed when
                                   popping the buffer.  */

  struct cpp_buffer *prev;

  /* Pointer into the file table; non-NULL if this is a file buffer.
     Used for include_next and to record control macros.  */
  struct _cpp_file *file;

  /* Saved value of __TIMESTAMP__ macro - date and time of last modification
     of the assotiated file.  */
  const unsigned char *timestamp;

  /* Value of if_stack at start of this file.
     Used to prohibit unmatched #endif (etc) in an include file.  */
  struct if_stack *if_stack;

  /* True if we need to get the next clean line.  */
  bool need_line : 1;

  /* True if we have already warned about C++ comments in this file.
     The warning happens only for C89 extended mode with -pedantic on,
     or for -Wtraditional, and only once per file (otherwise it would
     be far too noisy).  */
  bool warned_cplusplus_comments : 1;

  /* True if we don't process trigraphs and escaped newlines.  True
     for preprocessed input, command line directives, and _Pragma
     buffers.  */
  bool from_stage3 : 1;

  /* At EOF, a buffer is automatically popped.  If RETURN_AT_EOF is
     true, a CPP_EOF token is then returned.  Otherwise, the next
     token from the enclosing buffer is returned.  */
  bool return_at_eof : 1;

  /* One for a system header, two for a C system header file that therefore
     needs to be extern "C" protected in C++, and zero otherwise.  */
  unsigned char sysp;

  /* The directory of the this buffer's file.  Its NAME member is not
     allocated, so we don't need to worry about freeing it.  */
  struct cpp_dir dir;
};

struct lexer_state {
  /* 1 if we're handling a directive.  2 if it's an include-like
     directive.  */
  unsigned char in_directive;

  /* Nonzero if in a directive that will handle padding tokens itself.
     #include needs this to avoid problems with computed include and
     spacing between tokens.  */
  unsigned char directive_wants_padding;

  /* True if we are skipping a failed conditional group.  */
  unsigned char skipping;

  /* Nonzero if in a directive that takes angle-bracketed headers.  */
  unsigned char angled_headers;

  /* Nonzero if in a #if or #elif directive.  */
  unsigned char in_expression;

  /* Nonzero to save comments.  Turned off if discard_comments, and in
     all directives apart from #define.  */
  unsigned char save_comments;

  /* Nonzero if lexing __VA_ARGS__ and __VA_OPT__ are valid.  */
  unsigned char va_args_ok;

  /* Nonzero if lexing poisoned identifiers is valid.  */
  unsigned char poisoned_ok;

  /* Nonzero to prevent macro expansion.  */
  unsigned char prevent_expansion;

  /* Nonzero when parsing arguments to a function-like macro.  */
  unsigned char parsing_args;

  /* Nonzero if prevent_expansion is true only because output is
     being discarded.  */
  unsigned char discarding_output;

  /* Nonzero to skip evaluating part of an expression.  */
  unsigned int skip_eval;

  /* Nonzero if CPP_COMMA is valid in expression in C++.  */
  unsigned int comma_ok;

  /* Nonzero when tokenizing a deferred pragma.  */
  unsigned char in_deferred_pragma;

  /* Count to token that is a header-name.  */
  unsigned char directive_file_token;

  /* Nonzero if the deferred pragma being handled allows macro expansion.  */
  unsigned char pragma_allow_expansion;

  /* Nonzero if _Pragma should not be interpreted.  */
  unsigned char ignore__Pragma;
};

/* A generic memory buffer, and operations on it.  */
struct _cpp_buff {
  struct _cpp_buff *next;
  unsigned char *base, *cur, *limit;
};

union utoken {
  const cpp_token *token;
  const cpp_token **ptoken;
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

/* A "run" of tokens; part of a chain of runs.  */
class tokenrun {
  tokenrun *next, *prev;

public:
  cpp_token *base, *limit;

  tokenrun(unsigned int count, tokenrun * cur){
      base = XNEWVEC (cpp_token, count);
      limit = base + count;
      next = NULL;
      prev = cur; 
  }

  tokenrun *next_tokenrun() {
    if (this->next == nullptr) {
      this->next = XNEW(tokenrun);
      *(this->next) = tokenrun(250, this);
    }

    return this->next;
  }
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

struct cpp_context {
  /* Doubly-linked list.  */
  cpp_context *next, *prev;

  /* For ISO macro expansion.  Contexts other than the base context
     are contiguous tokens.  e.g. macro expansions, expanded
     argument tokens.  */
  struct {
    union utoken first;
    union utoken last;
  } iso;
  /* If non-NULL, a buffer used for storage related to this context.
     When the context is popped, the buffer is released.  */
  _cpp_buff *buff;

  /* If tokens_kind is TOKEN_KIND_EXTENDED, then (as we thus are in a
     macro context) this is a pointer to an instance of macro_context.
     Otherwise if tokens_kind is *not* TOKEN_KIND_EXTENDED, then, if
     we are in a macro context, this is a pointer to an instance of
     cpp_hashnode, representing the name of the macro this context is
     for.  If we are not in a macro context, then this is just NULL.
     Note that when tokens_kind is TOKEN_KIND_EXTENDED, the memory
     used by the instance of macro_context pointed to by this member
     is de-allocated upon de-allocation of the instance of struct
     cpp_context.  */
  union {
    macro_context *mc;
    cpp_hashnode *macro;
  } c;

  /* This determines the type of tokens held by this context.  */
  enum context_tokens_kind tokens_kind;
};

/* A cpp_reader encapsulates the "state" of a pre-processor run.
   Applying cpp_get_token repeatedly yields a stream of pre-processor
   tokens.  Usually, there is only one cpp_reader object active.  */
class cpp_reader {
private:
  /* Top of buffer stack.  */
  cpp_buffer *buffer;

  /* Overlaid buffer (can be different after processing #include).  */
  cpp_buffer *overlaid_buffer;

  /* Lexer state.  */
  struct lexer_state state;

  /* The line of the '#' of the current directive.  */
  location_t directive_line;

  /* Context stack.  */
  struct cpp_context base_context;
  struct cpp_context *context;

  /* If in_directive, the directive if known.  */
  const struct directive *directive;

  /* Token generated while handling a directive, if any. */
  cpp_token directive_result;

  /* When expanding a macro at top-level, this is the location of the
     macro invocation.  */
  location_t invocation_location;

  /* This is the node representing the macro being expanded at
     top-level.  The value of this data member is valid iff
     cpp_in_macro_expansion_p() returns TRUE.  */
  cpp_hashnode *top_most_macro_node;

  /* Nonzero if we are about to expand a macro.  Note that if we are
     really expanding a macro, the function macro_of_context returns
     the macro being expanded and this flag is set to false.  Client
     code should use the function cpp_in_macro_expansion_p to know if we
     are either about to expand a macro, or are actually expanding
     one.  */
  bool about_to_expand_macro_p;

  /* True if the preprocessor should diagnose CPP_DOT or CPP_COLON
     tokens as the first ones coming from macro expansion.  */
  bool diagnose_dot_colon_from_macro_p;

  /* Search paths for include files.  */
  struct cpp_dir *quote_include;   /* "" */
  struct cpp_dir *bracket_include; /* <> */

  /* Chain of all hashed _cpp_file instances.  */
  struct _cpp_file *all_files;

  struct _cpp_file *main_file;

  /* File and directory hash table.  */
  struct htab *file_hash;
  struct htab *dir_hash;
  struct file_hash_entry_pool *file_hash_entries;

  /* Lexing.  */
  cpp_token *cur_token;
  tokenrun base_run, *cur_run;
  unsigned int lookaheads;

  /* Buffer to hold macro definition string.  */
  unsigned char *macro_buffer;
  unsigned int macro_buffer_len;

  /* A token demarking macro arguments.  */
  cpp_token endarg;

  /* Identifier hash table.  */
  struct ht *hash_table;

  /* Identifier ancillary data hash table.  */
  struct ht *extra_hash_table;

  /* Expression parser stack.  */
  struct op *op_stack, *op_limit;

  /* preprocessing output buffer (a logical line).  */
  struct {
    unsigned char *base;
    unsigned char *limit;
    unsigned char *cur;
    location_t first_line;
  } out;

  /* Location identifying the main source file -- intended to be line
     zero of said file.  */
  location_t main_loc;
  const cpp_token *_cpp_lex_token();

public:
  const cpp_token *get_token();
};

#endif
