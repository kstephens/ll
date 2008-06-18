#ifndef _rc4_h
#define _rc4_h

/* $Id: rc4.h,v 1.4 2000/05/10 03:55:22 stephensk Exp $ */

#include <stddef.h>

typedef struct rc4_state {
  unsigned char _state[256];
  unsigned char _x, _y;
} rc4_state;

#ifndef RC4_EXPORT
#define RC4_EXPORT
#endif

RC4_EXPORT int rc4_init(rc4_state *s, const char *key, size_t keylen);
RC4_EXPORT int rc4_crypt(rc4_state *s, char *data, size_t datalen);

#endif
