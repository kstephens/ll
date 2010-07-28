#include <stdio.h>
#include <assert.h>
#include <stdlib.h>


static 
int bit_counts[0x100];

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
  int i;
  for ( i = 0; i < 0x100; ++ i ) {
    bit_counts[i] = bit_count_simple(i);
  }
}


static
int bit_count_fast(unsigned long x)
{
  int count = 0;

  if ( sizeof(x) == 8 ) {
    count += bit_counts[x & 0xff];
    x >>= 8;
    count += bit_counts[x & 0xff];
    x >>= 8;
    count += bit_counts[x & 0xff];
    x >>= 8;
    count += bit_counts[x & 0xff];
    x >>= 8;
  }
  if ( sizeof(x) == 4 ) {
    count += bit_counts[x & 0xff];
    x >>= 8;
    count += bit_counts[x & 0xff];
    x >>= 8;
  }
  if ( sizeof(x) == 2 ) {
    count += bit_counts[x & 0xff];
    x >>= 8;
  }
  count += bit_counts[x & 0xff];

  return count;
}

int main(int argc, char **argv)
{
  int n;
  int N = 1000000000;

  const char *mode = argc > 1 ? argv[1] : "simple";

  bit_counts_setup();

  if ( ! strcmp(mode, "time") ) {
    (void) system("set -x; time ./count_bits simple");
    (void) system("set -x; time ./count_bits fast");
    (void) system("set -x; time ./count_bits gcc_popcount");
  }
  else if ( ! strcmp(mode, "simple") ) {
    int bits = 0;
    for ( n = 0; n < N; ++ n ) {
      bits += bit_count_simple(n);
    }
    printf("bits = %d\n", bits);
  }
  else if ( ! strcmp(mode, "fast") ) {
    int bits = 0;
    for ( n = 0; n < N; ++ n ) {
      bits += bit_count_fast(n);
    }
    printf("bits = %d\n", bits);
  }
  else if ( ! strcmp(mode, "gcc_popcount") ) {
    int bits = 0;
    for ( n = 0; n < N; ++ n ) {
      bits += __builtin_popcount(n);
    }
    printf("bits = %d\n", bits);
  }
  else if ( ! strcmp(mode, "check") ) {
    for ( n = 0; n < N; ++ n ) {
      assert(bit_count_fast(n) == bit_count_simple(n));
      assert(__builtin_popcount(n) == bit_count_simple(n));
    }
    printf("check OK\n");
  }
  else {
    abort();
  }

  return 0;
}
