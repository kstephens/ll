#include <stdlib.h>
#include <stddef.h>
#include <string.h>

static
unsigned int inthash(int _x)
{
  const unsigned char *x = (const char*) &_x;
  unsigned int hash = x[0];

#define DO_HASH(i) hash ^= (hash << 15) | (hash >> 1) ^ x[i]
  switch ( sizeof(_x) ) {
  default:
  case 4:
    DO_HASH(3);
  case 3:
    DO_HASH(2);
  case 2:
    DO_HASH(1);
  }
#undef DO_HASH

  return hash;
}

#include "int_voidP_Table.def"
#include "hash.c"

