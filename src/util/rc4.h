#ifndef _rc4_h
#define _rc4_h

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
