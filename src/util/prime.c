#include "prime.h"

#ifdef UNIT_TEST
#include <stdio.h>
#endif

#include <stdlib.h>
#include <assert.h>
#ifdef MEMDEBUG
#include "memdebug/memdebug.h"
#endif

#ifndef EXTERN
#define EXTERN
#endif

struct prime_list {
  prime_t x;
  struct prime_list *next;
};

static struct prime_list 
  pl_3 = { 3 },
  pl_2 = { 2, &pl_3 },
  *pl = &pl_2,
  *pl_last = &pl_3;

static const prime_t *prime_new(prime_t x)
{
  struct prime_list *p;

#ifdef UNIT_TEST
  assert(pl_last->x != x);
  /* Do a simple verification of the primality of x. */
  {
    prime_t y = x / 2;
    while ( y >= 2 ) {
      assert( x % y );
      y --;
    }
  }
  fprintf(stderr, "%lu is prime\n", (unsigned long) x);
#endif

  p = malloc(sizeof(*p));
  p->x = x;
  p->next = 0;
  pl_last->next = p;
  pl_last = p;

  return &(p->x);
}

EXTERN const prime_t *prime_next(const prime_t *x) /* <DOC FUNC>Returns the next prime > ^x^.   ^x^ must be a result returned from prime_*(). */
{
  struct prime_list *p = (struct prime_list *) x;

  if ( ! p ) 
    return 0;

  if ( p->next ) {
    return &(p->next->x);

  } else {
    prime_t y;

    assert(p == pl_last);

#ifdef UNIT_TEST
    fprintf(stderr, "prime_next(%lu)\n", (unsigned long) *x);
#endif

    y = p->x + 2;
#ifdef UNIT_TEST
    /* Make sure y is not even. */
    assert(y % 2 != 0);
#endif
    for ( p = &pl_3; p; ) {
      if ( y % p->x == 0 ) {
	/* y is not prime, try y + 2. */
	y += 2;
	p = &pl_3;
	continue;
      } else {
	/* y might be prime, see if y is multiple of next prime. */
	p = p->next;
      }
    }
      
    /* y must be prime */
  
    return prime_new(y);
  }
}

EXTERN const prime_t *prime_le(prime_t x) /* <DOC FUNC>Returns a prime <= ^x^. */
{
#ifdef UNIT_TEST
  fprintf(stderr, "prime_le(%lu)\n", (unsigned long) x);
#endif

  if ( x < 2 )
    return 0;

  while ( pl_last->x < x ) {
    prime_next(&(pl_last->x));
  }

  {
    struct prime_list *p, *pp = pl;
    
    for ( p = pp->next; p; pp = p, p = p->next ) {
      if ( p->x > x ) {
	return &pp->x;
      }
    }

    abort();
  }
}


EXTERN const prime_t *prime_q(prime_t x) /* <DOC FUNC>Returns a prime if ^x^ is prime, 0 if ^x^ is not prime. */
{
  if ( x == pl_last->x ) {
    return &(pl_last->x);
  } else if ( x > pl_last->x ) {
    prime_le(x);
  }

  {
    struct prime_list *p;

    for ( p = pl; p; p = p->next ) {
      if ( p->x == x ) {
	/* x is prime. */
	return &p->x;
      }
    }

    return 0;
  }
}

EXTERN const prime_t *prime_ge(prime_t x) /* <DOC FUNC>Returns a prime number greater than or equal to ^x^. */
{
#ifdef UNIT_TEST
  fprintf(stderr, "prime_ge(%lu)\n", (unsigned long) x);
#endif

  while ( pl_last->x < x ) {
    prime_next(&(pl_last->x));
  }

  {
    struct prime_list *p;
    
    for ( p = pl; p; p = p->next ) {
      if ( p->x >= x ) {
	return &(p->x);
      }
    }

    abort();
  }
}

EXTERN prime_t *prime_factors(prime_t x)
{
  int nfactors = 0;
  prime_t *factors = malloc(sizeof(factors[0]) * (nfactors + 1));
  prime_t *f = pl;

#ifdef UNIT_TEST
  prime_t x_save = x;
#endif

  /*
  ** For each prime f < x:
  */
  for ( f = pl; *f <= x; f = prime_next(f) ) {
    /*
    ** If x is divisible by f,
    ** append f to factors list.
    */
    while ( x % *f == 0 ) {
      factors = realloc(factors, sizeof(factors[0]) * (nfactors + 2));
      factors[nfactors ++] = *f;
      x /= *f;
    }
  }

  factors[nfactors] = 0;

#ifdef UNIT_TEST
  {
    int i;
    prime_t product = 1;
    x = x_save;
    for ( i = 0; factors[i]; ++ i ) {
      product *= factors[i];
    }
    assert(product == x);
  }
#endif

  return factors;
}

#undef EXTERN

#ifdef UNIT_TEST
int main(int argc, char **argv)
{

#define Pi(X) {prime_t _X = (X); printf("%s = %lu\n", #X, (unsigned long) _X);}
  Pi(*prime_le(1024));
  Pi(*prime_ge(1024));
  Pi(*prime_le(512));
  Pi(*prime_ge(512));

  {
    int i, j;
    prime_t xs[] = { 678, 231, 148, 829, 818, 131, 512, 569, 126, 1025 };

    for ( i = 1; i < sizeof(xs)/sizeof(xs[0]); ++ i ) {
      prime_t x = xs[i];
      prime_t *factors = prime_factors(x);

      printf("factors(%lu) = { ", (unsigned long) x);
      for ( j = 0; factors[j]; ++ j ) {
	printf("%lu%s ", (unsigned long) factors[j], 
	       factors[j + 1] ? "," : "");
      }
      printf("}\n");

      free(factors);
    }
  }

  return 0;
}
#endif

