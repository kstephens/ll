static
unsigned int int_hash(int x)
{
  unsigned int hash = (unsigned int) x;

#define DO_HASH(i) hash ^= ((hash << 15) | (hash >> 1)); hash ^= (x >> (8 * i)) & 0xff;
  switch ( sizeof(x) ) {
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

  return hash;
}

