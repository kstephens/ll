#ifndef HASH_EXTERN
#define HASH_EXTERN
#endif

static unsigned int strhash (const char *str)
{
  unsigned int hash = 0;
  while ( *str ) {
    hash ^= * (unsigned char*) (str ++);
    hash ^= (hash << 15) | (hash >> 1);
  }
  return hash;
}

