
#include <iostream>

#include "system.h"

[[noreturn]] void fancy_abort(const char *file, int line,
                              const char *function) {

  std::cout << "Program aborted!\n"
            << "File: " << file << "\n"
            << "Line: " << line << "\n"
            << "Function: " << function << "\n";

  std::terminate(); // завершает программу сразу
}
