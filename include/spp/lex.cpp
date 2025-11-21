#include "spplib.h"
#include <cstdint>

#define UC (const unsigned char *)
typedef unsigned char uchar;

enum spell_type { SPELL_OPERATOR = 0, SPELL_IDENT, SPELL_LITERAL, SPELL_NONE };

struct token_spelling {
  enum spell_type category;
  const unsigned char *name;
};

#include "ttype_table.h"

#define OP(e, s) {SPELL_OPERATOR, UC s},
#define TK(e, s) {SPELL_##s, UC #e},
static const struct token_spelling token_spellings[N_TTYPES] = {TTYPE_TABLE};
#undef OP
#undef TK

#define TOKEN_SPELL(token) (token_spellings[(token)->type].category)
#define TOKEN_NAME(token) (token_spellings[(token)->type].name)

/* A version of the fast scanner using bit fiddling techniques.

   For 32-bit words, one would normally perform 16 comparisons and
   16 branches.  With this algorithm one performs 24 arithmetic
   operations and one branch.  Whether this is faster with a 32-bit
   word size is going to be somewhat system dependent.

   For 64-bit words, we eliminate twice the number of comparisons
   and branches without increasing the number of arithmetic operations.
   It's almost certainly going to be a win with 64-bit word size.  */

typedef uint64_t word_type;

/* Return X with the first N bytes forced to values that won't match one
   of the interesting characters.  Note that NUL is not interesting.  */

static inline word_type acc_char_mask_misalign(word_type val, unsigned int n) {
  word_type mask = -1;
  mask <<= n * 8;
  return val & mask;
}

/* Return X replicated to all byte positions within WORD_TYPE.  */

static inline word_type acc_char_replicate(uchar x) {
  word_type ret;

  ret = (x << 24) | (x << 16) | (x << 8) | x;
  if (sizeof(word_type) == 8)
    ret = (ret << 16 << 16) | ret;
  return ret;
}

/* Return non-zero if some byte of VAL is (probably) C.  */

static inline word_type acc_char_cmp(word_type val, word_type c) {
  word_type magic = 0x7efefefeU;
  if (sizeof(word_type) == 8)
    magic = (magic << 16 << 16) | 0xfefefefeU;
  magic |= 1;

  val ^= c;
  return ((val + magic) ^ ~val) & ~magic;
}

/* Given the result of acc_char_cmp is non-zero, return the index of
   the found character.  If this was a false positive, return -1.  */

static inline int acc_char_index(word_type cmp, word_type val) {
  unsigned int i;

  /* ??? It would be nice to force unrolling here,
     and have all of these constants folded.  */
  for (i = 0; i < sizeof(word_type); ++i) {
    uchar c;
    c = (val >> i * 8) & 0xff;

    if (c == '\n' || c == '\r' || c == '\\' || c == '?')
      return i;
  }

  return -1;
}

static inline const uchar *search_line_acc_char(const uchar *s,
                                                const uchar *end) {
  const word_type repl_nl = acc_char_replicate('\n');
  const word_type repl_cr = acc_char_replicate('\r');
  const word_type repl_bs = acc_char_replicate('\\');
  const word_type repl_qm = acc_char_replicate('?');

  unsigned int misalign;
  const word_type *p;
  word_type val, t;

  /* Align the buffer.  Mask out any bytes from before the beginning.  */
  p = (word_type *)((uintptr_t)s & -sizeof(word_type));
  val = *p;
  misalign = (uintptr_t)s & (sizeof(word_type) - 1);
  if (misalign)
    val = acc_char_mask_misalign(val, misalign);

  /* Main loop.  */
  while (1) {
    t = acc_char_cmp(val, repl_nl);
    t |= acc_char_cmp(val, repl_cr);
    t |= acc_char_cmp(val, repl_bs);
    t |= acc_char_cmp(val, repl_qm);

    if (__builtin_expect(t != 0, 0)) {
      int i = acc_char_index(t, val);
      if (i >= 0)
        return (const uchar *)p + i;
    }

    val = *++p;
  }
}


