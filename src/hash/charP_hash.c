#ifndef HASH_EXTERN
#define HASH_EXTERN
#endif

static unsigned int strhash (const char *str)
{
  unsigned int hash = str[0];
  while ( *str ) {
    hash ^= ((hash << 15) | (hash >> 1));
    hash ^= * (unsigned char*) (str ++);
  }
  return hash;
}

