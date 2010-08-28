/* Copyright (c) 1998, 1999 Kurt A. Stephens and Ion, Inc. */

#ifndef _util_mem_h
#define _util_mem_h

void memcpy_fast(void *, const void *, unsigned int);
void memset_fast(void *, int, unsigned int);

#ifdef memcpy
#undef memcpy
#endif
#define memcpy(x,y,z) memcpy_fast(x,y,z)

#ifdef memset
#undef memset
#endif
#define memset(x,y,z) memset_fast(x,y,z)

#endif

