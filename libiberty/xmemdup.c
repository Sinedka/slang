#include "libiberty.h"

#include <string.h>
#include <sys/types.h>

void *xmemdup(const void *input, size_t copy_size, size_t alloc_size) {
  void *output = xmalloc(alloc_size);
  if (alloc_size > copy_size)
    memset((char *)output + copy_size, 0, alloc_size - copy_size);
  return (void *)memcpy(output, input, copy_size);
}
