#define TTYPE_TABLE                                                            \
  OP(EQ, "=")                                                                  \
  OP(NOT, "!")                                                                 \
  OP(GREATER, ">") /* compare */                                               \
  OP(LESS, "<")                                                                \
  OP(PLUS, "+") /* math */                                                     \
  OP(MINUS, "-")                                                               \
  OP(MULT, "*")                                                                \
  OP(DIV, "/")                                                                 \
  OP(MOD, "%")                                                                 \
  OP(AND, "&") /* bit ops */                                                   \
  OP(OR, "|")                                                                  \
  OP(XOR, "^")                                                                 \
  OP(RSHIFT, ">>")                                                             \
  OP(LSHIFT, "<<")                                                             \
                                                                               \
  OP(COMPL, "~")                                                               \
  OP(AND_AND, "&&") /* logical */                                              \
  OP(OR_OR, "||")                                                              \
  OP(QUERY, "?")                                                               \
  OP(COLON, ":")                                                               \
  OP(COMMA, ",") /* grouping */                                                \
  OP(OPEN_PAREN, "(")                                                          \
  OP(CLOSE_PAREN, ")")                                                         \
  TK(EOF, NONE)                                                                \
  OP(EQ_EQ, "==") /* compare */                                                \
  OP(NOT_EQ, "!=")                                                             \
  OP(GREATER_EQ, ">=")                                                         \
  OP(LESS_EQ, "<=")                                                            \
                                                                               \
  /* These two are unary + / - in preprocessor expressions.  */                \
  OP(PLUS_EQ, "+=") /* math */                                                 \
  OP(MINUS_EQ, "-=")                                                           \
                                                                               \
  OP(MULT_EQ, "*=")                                                            \
  OP(DIV_EQ, "/=")                                                             \
  OP(MOD_EQ, "%=")                                                             \
  OP(AND_EQ, "&=") /* bit ops */                                               \
  OP(OR_EQ, "|=")                                                              \
  OP(XOR_EQ, "^=")                                                             \
  OP(RSHIFT_EQ, ">>=")                                                         \
  OP(LSHIFT_EQ, "<<=")                                                         \
  /* Digraphs together, beginning with CPP_FIRST_DIGRAPH.  */                  \
  OP(HASH, "#") /* digraphs */                                                 \
  OP(PASTE, "##")                                                              \
  OP(OPEN_SQUARE, "[")                                                         \
  OP(CLOSE_SQUARE, "]")                                                        \
  OP(OPEN_BRACE, "{")                                                          \
  OP(CLOSE_BRACE, "}")                                                         \
  /* The remainder of the punctuation. Order is not significant.*/             \
  OP(SEMICOLON, ";") /* structure */                                           \
  OP(ELLIPSIS, "...")                                                          \
  OP(PLUS_PLUS, "++") /* increment */                                          \
  OP(MINUS_MINUS, "--")                                                        \
  OP(DEREF, "->") /* accessors */                                              \
  OP(DOT, ".")                                                                 \
  OP(SCOPE, "::")                                                              \
                                                                               \
  TK(NAME, IDENT)     /* word */                                               \
  TK(NUMBER, LITERAL) /* 34_be+ta  */                                          \
                                                                               \
  TK(CHAR, LITERAL) /* 'char' */                                               \
  TK(OTHER,		LITERAL) /* stray punctuation */		                             \
                                                                               \
  TK(STRING, LITERAL)      /* "string" */                                      \
  TK(HEADER_NAME, LITERAL) /* <stdio.h> in #include */                         \
                                                                               \
  TK(MACRO_ARG, NONE) /* Macro argument.  */                                   \
  TK(PADDING, NONE)   /* Whitespace for -E.	*/
