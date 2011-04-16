#include <stdlib.h>
#include <stddef.h>
#include <string.h>

static
unsigned int voidP_hash(void *_x)
{
  const unsigned char *x = (const char*) &_x;
  unsigned int hash = x[0];

#define DO_HASH(i) hash ^= (hash << 15) | (hash >> 1) ^ x[i]
  switch ( sizeof(_x) ) {
  case 8:
    DO_HASH(7);
  case 7:
    DO_HASH(6);
  case 6:
    DO_HASH(5);
  case 5:
    DO_HASH(4);
  case 4:
    DO_HASH(3);
  case 3:
    DO_HASH(2);
  case 2:
    DO_HASH(1);
  default:
    abort();
  }
#undef DO_HASH

  return hash;
}

#include "voidP_voidP_Table.def"
#include "hash.c"

