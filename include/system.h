[[noreturn]] void fancy_abort(const char *file, int line, const char *function);

#ifdef RELEASE_BUILD
#define abort() std::abort
#else
#define abort() fancy_abort(__FILE__, __LINE__, __FUNCTION__)
#endif

typedef unsigned char uchar;

#define HT_HASHSTEP(r, c) ((r) * 67 + ((c) - 113));
#define HT_HASHFINISH(r, len) ((r) + (len))


#define CPP_HASHNODE(HNODE)	((cpp_hashnode *) (HNODE))
#define HT_NODE(NODE)		(&(NODE)->ident)
#define NODE_LEN(NODE)		HT_LEN (HT_NODE (NODE))
#define NODE_NAME(NODE)		HT_STR (HT_NODE (NODE))

