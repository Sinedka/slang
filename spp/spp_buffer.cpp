#include "spplib.h"
#include <cstring>

bool spp_buffer::get_fresh_line() {
  if (!line_begin)
    delete[] line_begin;
  // file_cur++;
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
spp_buffer::spp_buffer(char *filename) {
  line_begin = nullptr;
  file = new spp_file(filename);
  file_cur = file->content;
}

spp_buffer::~spp_buffer() {
  delete file;
  delete[] line_begin;
}

spp_buffer *spp_buffer::get_prev() { return prev; }
