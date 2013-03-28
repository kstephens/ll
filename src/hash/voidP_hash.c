static
unsigned int voidP_hash(void *_x)
{
  size_t x = (size_t) _x;
  size_t hash = x;

#define DO_HASH(i) hash ^= ((hash << 15) | (hash >> 1)); hash ^= (x >> (i * 8)) & 0xff;
  switch ( sizeof(_x) ) {
  default:
    abort();
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
  }
#undef DO_HASH

  return (unsigned int) hash;
}

