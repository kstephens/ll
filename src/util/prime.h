#ifndef _util_prime_h
#define _util_prime_h

#ifndef prime_t
typedef unsigned long prime_t;
#endif

const prime_t * prime_le(prime_t x);
const prime_t * prime_ge(prime_t x);
const prime_t * prime_q(prime_t x);
const prime_t * prime_next(const prime_t *p);
prime_t * prime_factors(prime_t x); /* must free() result. */

#endif
