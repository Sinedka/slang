#include "spplib.h"
#include <cstring>

bool cpp_buffer::get_fresh_line() {
  if (!line_begin)
    delete[] line_begin;
  file_cur++;
  if (*file_cur == '\0')
    return false;


  char *beg = file_cur;

  while (*file_cur != '\n' || !file_cur) {
    file_cur++;
  }


  line_begin = new char[file_cur - beg + 2];
  memcpy(line_begin, beg, file_cur - beg);
  line_begin[file_cur - beg] = '\n';
  line_begin[file_cur - beg + 1] = '\0';
  cur = line_begin;
  file_cur++;

  return true;
}
cpp_buffer::cpp_buffer(char *filename) {
  line_begin = nullptr;
  file = new cpp_file(filename);
  file_cur = file->content-1;
}

cpp_buffer::~cpp_buffer() {
  delete file;
  delete[] line_begin;
}

cpp_buffer *cpp_buffer::get_prev() { return prev; }
