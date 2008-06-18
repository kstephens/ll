#ifndef _util_MEMCPY_H
#define _util_MEMCPY_H

#ifdef __cplusplus

#include <assert.h>

/* Typesafe memcpy handles assignment properly. */
template<class T1, class T2> inline void _typesafe_memcpy(T1 *_dst, const T2 *_src, size_t _size)
{
  assert(_size % sizeof(*_dst) == 0);
  _size /= sizeof(*_dst);
  while ( _size -- >= 0 ) {
    *(_dst ++) = *(_src ++);
  }
}

#define memcpy(A,B,C) _typesafe_memcpy(A,B,C)

#else

#include <string.h>

#endif

#endif
