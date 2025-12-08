#ifndef LIBIBERTY_H
#define LIBIBERTY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#include <stddef.h>

/* Report an allocation failure.  */
extern void xmalloc_failed(size_t) __attribute__((noreturn));

/* Allocate memory without fail.  Prints an error and exits.  */
extern void *xmalloc(size_t) __attribute__((malloc))
__attribute__((returns_nonnull)) __attribute__((warn_unused_result));

/* Reallocate memory without fail.  */
extern void *xrealloc(void *, size_t) __attribute__((returns_nonnull))
__attribute__((warn_unused_result));

/* Allocate zeroed memory without fail.  */
extern void *xcalloc(size_t, size_t) __attribute__((malloc))
__attribute__((returns_nonnull)) __attribute__((warn_unused_result));

/* Duplicate memory buffer without fail.  */
extern void *xmemdup(const void *, size_t, size_t) __attribute__((malloc))
__attribute__((returns_nonnull)) __attribute__((warn_unused_result));

extern void xexit(int status) __attribute__((noreturn));

/* Scalar allocators.  */

#define XALLOCA(T) ((T *)alloca(sizeof(T)))
#define XNEW(T) ((T *)xmalloc(sizeof(T)))
#define XCNEW(T) ((T *)xcalloc(1, sizeof(T)))
#define XDUP(T, P) ((T *)xmemdup((P), sizeof(T), sizeof(T)))
#define XDELETE(P) free((void *)(P))

/* Array allocators.  */

#define XALLOCAVEC(T, N) ((T *)alloca(sizeof(T) * (N)))
#define XNEWVEC(T, N) ((T *)xmalloc(sizeof(T) * (N)))
#define XCNEWVEC(T, N) ((T *)xcalloc((N), sizeof(T)))
#define XDUPVEC(T, P, N) ((T *)xmemdup((P), sizeof(T) * (N), sizeof(T) * (N)))
#define XRESIZEVEC(T, P, N) ((T *)xrealloc((void *)(P), sizeof(T) * (N)))
#define XDELETEVEC(P) free((void *)(P))

/* Allocators for variable-sized structures and raw buffers.  */

#define XALLOCAVAR(T, S) ((T *)alloca((S)))
#define XNEWVAR(T, S) ((T *)xmalloc((S)))
#define XCNEWVAR(T, S) ((T *)xcalloc(1, (S)))
#define XDUPVAR(T, P, S1, S2) ((T *)xmemdup((P), (S1), (S2)))
#define XRESIZEVAR(T, P, S) ((T *)xrealloc((P), (S)))

#ifdef __cplusplus
}
#endif

#endif /* ! defined (LIBIBERTY_H) */
