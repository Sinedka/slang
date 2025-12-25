#include "spplib.h"
#include <cstring>

bool cpp_buffer::get_fresh_line() {
  if (!line_begin)
    delete[] line_begin;
  if (*file_cur == '\0')
    return false;

  file_cur++;

  char *beg = file_cur;

  while (*file_cur != '\n' || !file_cur) {
    file_cur++;
  }

  line_begin = new char[file_cur - beg + 2];
  memcpy(line_begin, beg, file_cur - beg);
  line_begin[file_cur - beg] = '\n';
  line_begin[file_cur - beg + 1] = '\0';
  cur = line_begin;
  return true;
}
cpp_buffer::cpp_buffer(char * filename) {
  line_begin = nullptr;
  file = new cpp_file(filename);
}

cpp_buffer::~cpp_buffer() {
  delete file;
  delete[] line_begin;
}

