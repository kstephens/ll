/* Copyright (c) 1998, 1999 Kurt A. Stephens and Ion, Inc. */

#ifndef _util_mem_h
#define _util_mem_h

#ifndef __rcs_id__
#ifndef __rcs_id_util_mem_h__
#define __rcs_id_util_mem_h__
static const char __rcs_id_util_mem_h[] = "$Id: mem.h,v 1.5 1999/06/28 14:04:14 stephensk Exp $";
#endif
#endif /* __rcs_id__ */

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

