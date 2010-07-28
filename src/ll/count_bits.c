#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>


#ifndef BC_T
#define BC_T char
#endif

static 
BC_T bit_counts[0x10000];

static 
int bit_count_simple(unsigned long x)
{
  int count = 0;
  while ( x ) {
    if ( x & 1 ) ++ count;
    x >>= 1;
  }
  return count;
}

static 
void bit_counts_setup()
{
  unsigned int i;
  for ( i = 0; i < sizeof(bit_counts) / sizeof(bit_counts[0]); ++ i ) {
    bit_counts[i] = bit_count_simple(i);
    // fprintf(stderr, "  %x -> %d\n", i, bit_counts[i]);
  }
}


static
int bit_count_fast_8(unsigned long x)
{
  int count = 0;

  switch ( sizeof(x) ) {
  case 8:
    count += bit_counts[x & 0xff];
    x >>= 8;
    count += bit_counts[x & 0xff];
    x >>= 8;
    count += bit_counts[x & 0xff];
    x >>= 8;
    count += bit_counts[x & 0xff];
    x >>= 8;
  case 4:
    count += bit_counts[x & 0xff];
    x >>= 8;
    count += bit_counts[x & 0xff];
    x >>= 8;
  case 2:
    count += bit_counts[x & 0xff];
    x >>= 8;
  case 1:
    count += bit_counts[x & 0xff];
  }

  return count;
}


static
int bit_count_fast_16(unsigned long x)
{
  int count = 0;

  switch ( sizeof(x) ) {
  case 8:
    count += bit_counts[x & 0xffff];
    x >>= 16;
    count += bit_counts[x & 0xffff];
    x >>= 16;
  case 4:
    count += bit_counts[x & 0xffff];
    x >>= 16;
  case 2:
    count += bit_counts[x & 0xffff];
    break;
  case 1:
    count += bit_counts[x & 0xff];
  }

  return count;
}


int main(int argc, char **argv)
{
  int n;
#ifndef N 
#define N 1000000000
#endif
  int (*f)(unsigned long) = 0;

  const char *mode = argc > 1 ? argv[1] : "simple";

  bit_counts_setup();

  if ( ! strcmp(mode, "time") ) {
    (void) system("set -x; time ./count_bits simple");
    (void) system("set -x; time ./count_bits fast_8");
    (void) system("set -x; time ./count_bits fast_16");
    (void) system("set -x; time ./count_bits gcc_popcount");
  }
  else if ( ! strcmp(mode, "simple") ) {
    f = bit_count_simple;
  }
  else if ( ! strcmp(mode, "fast_8") ) {
    f = bit_count_fast_8
  }
  else if ( ! strcmp(mode, "fast_16") ) {
    f = bit_count_fast_16;
  }
  else if ( ! strcmp(mode, "gcc_popcount") ) {
    unsigned int bits = 0;
    for ( n = 0; n < N; ++ n ) {
      bits += __builtin_popcount(n);
    }
    printf("bits = %u\n", bits);
  }
  else if ( ! strcmp(mode, "check") ) {
    for ( n = 0; n < N; ++ n ) {
      assert(bit_count_fast_8(n) == bit_count_simple(n));
      assert(bit_count_fast_16(n) == bit_count_simple(n));
      assert(__builtin_popcount(n) == bit_count_simple(n));
    }
    printf("check OK\n");
  }
  else {
    abort();
  }

  if ( f ) {
    unsigned int bits = 0;
    for ( n = 0; n < N; ++ n ) {
      bits += f(n);
    }
    printf("bits = %u\n", bits);
  }


  return 0;
}
