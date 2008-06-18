
#ifndef __rcs_id__
#ifndef __rcs_id_hash_voidP_voidP_Table_c__
#define __rcs_id_hash_voidP_voidP_Table_c__
static const char __rcs_id_hash_voidP_voidP_Table_c[] = "$Id: voidP_voidP_Table.c,v 1.2 1999/11/04 10:38:30 stephensk Exp $";
#endif
#endif /* __rcs_id__ */

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

#include "voidP_voidP_Table.def"
#include "hash.c"

