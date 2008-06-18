#ifndef __rcs_id__
#ifndef __rcs_id_hash_charP_hash_c__
#define __rcs_id_hash_charP_hash_c__
static const char __rcs_id_hash_charP_hash_c[] = "$Id: charP_hash.c,v 1.2 2001/06/15 07:28:51 stephens Exp $";
#endif
#endif /* __rcs_id__ */

#ifndef HASH_EXTERN
#define HASH_EXTERN
#endif

HASH_EXTERN unsigned int strhash (const char *str)
{
  unsigned int hash = 0;
  while ( *str ) {
    hash ^= * (unsigned char*) (str ++);
    hash ^= (hash << 15) | (hash >> 1);
  }
  return hash;
}

