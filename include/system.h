[[noreturn]] void fancy_abort(const char *file, int line, const char *function);

#ifdef RELEASE_BUILD
#define abort() std::abort
#else
#define abort() fancy_abort(__FILE__, __LINE__, __FUNCTION__)
#endif
