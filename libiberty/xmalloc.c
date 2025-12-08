
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "environ.h"
#include "libiberty.h"

#include <stddef.h>
#include <stdio.h>

#include <stdlib.h>
#include <unistd.h>

static char *first_break = NULL;

void xmalloc_failed(size_t size) {
  size_t allocated;

  if (first_break != NULL)
    allocated = (char *)sbrk(0) - first_break;
  else
    allocated = (char *)sbrk(0) - (char *)&environ;
  fprintf(stderr,
          "\nout of memory allocating %lu bytes after a total of %lu bytes\n",
          (unsigned long)size, (unsigned long)allocated);
  xexit(1);
}

void *xmalloc(size_t size) {
  void *newmem;

  if (size == 0)
    size = 1;
  newmem = malloc(size);
  if (!newmem)
    xmalloc_failed(size);

  return (newmem);
}

void *xcalloc(size_t nelem, size_t elsize) {
  void *newmem;

  if (nelem == 0 || elsize == 0)
    nelem = elsize = 1;

  newmem = calloc(nelem, elsize);
  if (!newmem)
    xmalloc_failed(nelem * elsize);

  return (newmem);
}

void *xrealloc(void *oldmem, size_t size) {
  void *newmem;

  if (size == 0)
    size = 1;
  if (!oldmem)
    newmem = malloc(size);
  else
    newmem = realloc(oldmem, size);
  if (!newmem)
    xmalloc_failed(size);

  return (newmem);
}
