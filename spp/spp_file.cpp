#include "spplib.h"
#include <cstdio>
#include <cstdlib>
#include <iostream>

cpp_file::cpp_file(const std::string &fname)
    : filename(fname), content(nullptr), length(0) {
  FILE *f = fopen(fname.c_str(), "rb");
  if (!f) {
    std::cerr << "Ошибка открытия файла: " << fname << "\n";
    return;
  }

  size_t cap = 1024;
  content = static_cast<char *>(malloc(cap));
  if (!content) {
    std::cerr << "Ошибка выделения памяти\n";
    fclose(f);
    return;
  }

  int c;
  while ((c = fgetc(f)) != EOF) {
    if (length + 2 >= cap) {
      cap *= 2;
      char *tmp = static_cast<char *>(realloc(content, cap));
      if (!tmp) {
        std::cerr << "Ошибка перераспределения памяти\n";
        if (content != nullptr) {
          free(content);
        }
        content = nullptr;
        fclose(f);
        return;
      }
      content = tmp;
    }

    if ((unsigned char)c < 128) {
      std::cout << static_cast<char>(c);
      content[length++] = static_cast<char>(c);
    } else {
      content[length++] = 0xff; // unknown
    }
  }

  content[length] = '\0';
  fclose(f);
}

cpp_file::~cpp_file() { free(content); }
