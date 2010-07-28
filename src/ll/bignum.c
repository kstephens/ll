#include "ll.h"
#include <string.h> /* strlen() */

/************************************************************************/

#define X  ll_SELF
#define Xr ll_THIS
#define Xn ll_THIS->_n

#define _n(X) ll_THIS_ISA(bignum, X)->_n

#define Yr ll_THIS_ISA(bignum, Y)
#define Yn Yr->_n

#define Rn ll_THIS_ISA(bignum, r)->_n

/************************************************************************/


#if 0
void
mpz_init_set_lt (mpz_ptr dest, mp_limb_t val)
{
  mp_size_t size;
  mp_limb_t vl;

  dest->_mp_alloc = 1;
  dest->_mp_d = (mp_ptr) (*__gmp_allocate_func) (BYTES_PER_MP_LIMB);

  vl = (mp_limb_t) (val >= 0 ? val : -val);

  dest->_mp_d[0] = vl & GMP_NUMB_MASK;
  size = vl != 0;

#if GMP_NAIL_BITS != 0
  if (vl > GMP_NUMB_MAX)
    {
      MPZ_REALLOC (dest, 2);
      dest->_mp_d[1] = vl >> GMP_NUMB_BITS;
      size = 2;
    }
#endif

  dest->_mp_size = val >= 0 ? size : -size;
}
#endif


static int _ll_initialized_bignum;

ll_v ll_make_bignum_(ll_v_word i)
{
  ll_v n;

  if ( ! _ll_initialized_bignum ) {
    ll_abort("ll_make_bignum_(): bignum not initialized");
  }

  n = ll_call(ll_o(make), _1(ll_type(bignum)));
  if ( sizeof(ll_v_word) > sizeof(long) ) {
#if 1
    mpz_init_set_si(_n(n), i);
    if ( i < 0 ) i = - i;
    _n(n)->_mp_d[0] = i;
#else
    /* portable but slow on 64-bit */
    mpz_t long_shift;
    mpz_init(long_shift);
    mpz_ui_pow_ui(long_shift, 2, (sizeof(long) * 8));
    
    /* Avoid bignum.c:28: error: right shift count >= width of type. */
#if 1
    mpz_init_set_si(_n(n), (long) ((((long long) i) >> (sizeof(long) * 4)) >> (sizeof(long) * 4)));
#else
    mpz_init_set_si(_n(n), (long) ((((long long) i) >> (sizeof(long) * 8)))));
#endif
  
    mpz_mul(_n(n), _n(n), long_shift);

    mpz_init_set_ui(long_shift, (unsigned long) i);
    mpz_add(_n(n), _n(n), long_shift);
  
    mpz_clear(long_shift);
#endif

  } else {
    mpz_init_set_si(_n(n), i);
  }

  return n;
}


ll_v ll_coerce_bignum(ll_v n)
{
  ll_v r;

  if ( ll_ISA_bignum(n) ) {
    return n;
  }
  else if ( ll_ISA_fixnum(n) ) {
    r = ll_make_bignum_(ll_UNBOX_fixnum(n));
  }
  else if ( ll_ISA_flonum(n) ) {
    r = ll_make_bignum_(0);
    mpz_init_set_d(_n(r), ll_UNBOX_flonum(n));
  }
  else {
    return(_ll_typecheck(ll_type(number), &n));
  }

  return r;
}


ll_v ll_clone_bignum(ll_v bn)
{
  return ll_call(ll_o(clone), _1(bn));
}


#define Y ll_ARG_1

ll_define_primitive(bignum, initialize, __1(r, n), _1(no_side_effect, "#t"))
{
  mpz_init(Xn);
  if ( ll_ARGC > 1 ) {
    if ( ll_ISA_fixnum(Y) ) {
      /* FIXME */
    }
    else if ( ll_ISA_flonum(Y) ) {
      
    }
    else {
      ll_return(_ll_typecheck(ll_type(number), &ll_ARG_1));
    }
  }
  ll_return(ll_SELF);
}
ll_define_primitive_end


ll_define_primitive(bignum, initialize_clone, _2(r, src), _1(no_side_effect, "#t"))
{
  mpz_init_set(Xn, Yn);
  ll_return(ll_SELF);
}
ll_define_primitive_end



/* Return a fixnum if bignum is "small" enough to fit. */
ll_v ll_unify_bignum(ll_v bn)
{
  if ( mpz_cmp(_n(ll_g(SbignumCminS)), _n(bn)) <= 0 && mpz_cmp(_n(bn), _n(ll_g(SbignumCmaxS))) <= 0 ) {
    mp_limb_t l;

    int mp_size = _n(bn)->_mp_size;
    assert(-1 <= mp_size && mp_size <= 1);
    l = _n(bn)->_mp_d[0];

    if ( _n(bn)->_mp_size < 0 )
      l = - l;

    // fprintf(stderr, "\n  <bignum>:%%unify %0llx\n ", (unsigned long long) l);
    assert(sizeof(l) == sizeof(ll_v));
    return ll_BOX_fixnum(l);
  }
  return bn;
}


ll_define_primitive(bignum, _unify, _1(r), _1(no_side_effect, "#t"))
{
  ll_return(ll_unify_bignum(ll_SELF));
}
ll_define_primitive_end


/************************************************************************/


ll_define_primitive(bignum, negativeQ, _1(x), _1(no_side_effect, "#t"))
{
  ll_return(ll_make_boolean(mpz_sgn(Xn) < 0));
}
ll_define_primitive_end


ll_define_primitive(bignum, zeroQ, _1(x), _1(no_side_effect, "#t"))
{
  ll_return(ll_make_boolean(mpz_sgn(Xn) == 0));
}
ll_define_primitive_end


ll_define_primitive(bignum, positiveQ, _1(x), _1(no_side_effect, "#t"))
{
  ll_return(ll_make_boolean(mpz_sgn(Xn) > 0));
}
ll_define_primitive_end


ll_define_primitive(bignum, oddQ, _1(x), _1(no_side_effect, "#t"))
{
  ll_return(ll_make_boolean(mpz_odd_p(Xn)));
}
ll_define_primitive_end


ll_define_primitive(bignum, evenQ, _1(x), _1(no_side_effect, "#t"))
{
  ll_return(ll_make_boolean(mpz_even_p(Xn)));
}
ll_define_primitive_end


/************************************************************************/


ll_define_primitive(bignum, quotient, _2(n1, n2), _1(no_side_effect, "#t"))
{
  // FIXME
  ll_ARG_1 = ll_coerce_bignum(ll_ARG_1);
  ll_SELF = ll_clone_bignum(ll_SELF);
  mpz_div(Xn, Xn, Yn);
  ll_return(ll_unify_bignum(ll_SELF));
}
ll_define_primitive_end


ll_define_primitive(bignum, remainder, _2(n1, n2), _1(no_side_effect, "#t"))
{
  // FIXME
  ll_ARG_1 = ll_coerce_bignum(ll_ARG_1);
  ll_SELF = ll_clone_bignum(ll_SELF);
  mpz_mod(Xn, Xn, Yn);
  ll_return(ll_unify_bignum(ll_SELF));
}
ll_define_primitive_end


ll_define_primitive(bignum, modulo, _2(n1, n2), _1(no_side_effect, "#t"))
{
  // FIXME
  ll_ARG_1 = ll_coerce_bignum(ll_ARG_1);
  ll_SELF = ll_clone_bignum(ll_SELF);
  mpz_mod(Xn, Xn, Yn);
  ll_return(ll_unify_bignum(ll_SELF));
}
ll_define_primitive_end


/************************************************************************/


ll_define_primitive(bignum, exact__inexact, _1(z), _1(no_side_effect, "#t"))
{
  ll_return(ll_make_flonum(mpz_get_d(Xn)));
}
ll_define_primitive_end


/************************************************************************/


ll_define_primitive(bignum, number__string, __1(z, radix), _1(no_side_effect, "#t"))
{
  int r = ll_ARGC >= 2 ? ll_UNBOX_fixnum(ll_ARG_1) : 10;
  ll_v str = ll_make_copy_string(0, mpz_sizeinbase (Xn, r) + 2);
  mpz_get_str(ll_ptr_string(str), r, Xn);
  ll_set_len_string(str, strlen(ll_ptr_string(str)));
  ll_return(str);
}

ll_define_primitive_end


/************************************************************************/
/* numeric and relational subprims */


ll_define_primitive(bignum, abs, _1(x), _1(no_side_effect, "#t"))
{
  if ( ll_negativeQ(X) ) {
    ll_SELF = ll_clone_bignum(ll_SELF);
    mpz_neg(Xn, Xn);
  }
  ll_return(ll_SELF);
}
ll_define_primitive_end


ll_define_primitive(bignum, _NEG, _1(x), _1(no_side_effect, "#t"))
{
  ll_SELF = ll_clone_bignum(ll_SELF);
  mpz_neg(Xn, Xn);
  ll_return(ll_SELF);
}
ll_define_primitive_end


ll_define_primitive(bignum, _ADD, _2(x, y), _1(no_side_effect, "#t"))
{
  if ( ll_ISA_fixnum(Y) ) {
    ll_return(ll__ADD(ll_SELF, ll_coerce_bignum(Y)));
  }
  else if ( ll_ISA_flonum(Y) ) {
    ll_return(ll__ADD(ll_coerce_flonum(ll_SELF), Y));
  } 
  else if ( ll_ISA_ratnum(Y) ) {
    ll_return(ll__ADD(ll_coerce_ratnum(ll_SELF), Y));
  } 
  else if ( ll_ISA_bignum(Y) ) {
    ll_v r = ll_make_bignum_(0);
    mpz_add(Rn, Xn, Yn);
    ll_return(ll_unify_bignum(r));
    // ll_return(r);
  } 
  else {
    ll_return(_ll_typecheck(ll_type(number), &ll_ARG_1));
  }
}
ll_define_primitive_end


ll_define_primitive(bignum, _SUB, _2(x, y), _1(no_side_effect, "#t"))
{
  if ( ll_ISA_fixnum(Y) ) {
    ll_return(ll__SUB(ll_SELF, ll_coerce_bignum(Y)));
  } 
  else if ( ll_ISA_flonum(Y) ) {
    ll_return(ll__SUB(ll_coerce_flonum(ll_SELF), Y));
  } 
  else if ( ll_ISA_ratnum(Y) ) {
    ll_return(ll__SUB(ll_coerce_ratnum(ll_SELF), Y));
  } 
  else if ( ll_ISA_bignum(Y) ) {
    ll_v r = ll_make_bignum_(0);
    mpz_sub(Rn, Xn, Yn);
    ll_return(ll_unify_bignum(r));
  }
  else {
    ll_return(_ll_typecheck(ll_type(number), &ll_ARG_1)); 
  }
}
ll_define_primitive_end


ll_define_primitive(bignum, _MUL, _2(x, y), _1(no_side_effect, "#t"))
{
  if ( ll_ISA_fixnum(Y) ) {
    ll_return(ll__MUL(ll_SELF, ll_coerce_bignum(Y)));
  } 
  else if ( ll_ISA_flonum(Y) ) {
    ll_return(ll__MUL(ll_coerce_flonum(ll_SELF), Y));
  }
  else if ( ll_ISA_ratnum(Y) ) {
    ll_return(ll__MUL(ll_coerce_ratnum(ll_SELF), Y));
  } 
  else if ( ll_ISA_bignum(Y) ) {
    ll_v r = ll_make_bignum_(0);
    mpz_mul(Rn, Xn, Yn);
    ll_return(ll_unify_bignum(r));
  } 
  else {
    ll_return(_ll_typecheck(ll_type(number), &ll_ARG_1)); 
  }
}
ll_define_primitive_end


ll_define_primitive(bignum, _DIV, _2(x, y), _1(no_side_effect, "#t"))
{
  if ( ll_ISA_fixnum(Y) ) {
    ll_return(ll__DIV(ll_SELF, ll_coerce_bignum(Y)));
  } 
  else if ( ll_ISA_flonum(Y) ) {
    ll_return(ll__DIV(ll_coerce_flonum(ll_SELF), Y));
  }
  else if ( ll_ISA_ratnum(Y) ) {
    ll_return(ll__DIV(ll_coerce_ratnum(ll_SELF), Y));
  } 
  else if ( ll_ISA_bignum(Y) ) {
    if ( mpz_cmp_ui(Xn, 0) == 0) {
      ll_return(_ll_error(ll_re(divide_by_zero),
			  2,
			  ll_s(numerator), ll_SELF,
			  ll_s(denominator), ll_ARG_1));
    } 
    else if ( mpz_divisible_p(Xn, Yn) ) {
      ll_return(ll_make_ratnum_(ll_SELF, ll_ARG_1));
    } 
    else {
      ll_v r = ll_make_bignum_(0);
      mpz_div(Rn, Xn, Yn);
      ll_return(ll_unify_bignum(r));
    }
  } 
  else {
    ll_return(_ll_typecheck(ll_type(number), &ll_ARG_1)); 
  }
}
ll_define_primitive_end


/************************************************************************/

#define BOP(N,OP)
#define UOP(N,OP)
#define ROP(N,OP) \
ll_define_primitive(bignum, _##N, _2(n1, n2), _1(no_side_effect,"#t"))  \
{									\
  if ( ll_ISA_flonum(ll_ARG_1) ) {					\
    ll_return(ll_make_boolean(ll_coerce_flonum(ll_SELF) OP ll_UNBOX_flonum(ll_ARG_1))); \
  } else {								\
    ll_ARG_1 = ll_coerce_bignum(ll_ARG_1);				\
    ll_return(ll_make_boolean(mpz_cmp(Xn, Yn) OP 0));			\
  }									\
}									\
ll_define_primitive_end

#include "cops.h"

/************************************************************************/

#undef X
#undef Xn
#undef Y
#undef Yn
#undef Rn

/************************************************************************/


#define _ll_malloc_gmp ll_malloc

void * _ll_realloc_gmp(void *ptr, size_t old_size, size_t new_size)
{
  return ll_realloc(ptr, new_size);
}


void _ll_free_gmp(void *ptr, size_t size)
{
  ll_free(ptr);
}


ll_INIT(bignum, 255, "bignum init")
{
  mp_set_memory_functions(_ll_malloc_gmp,
			  _ll_realloc_gmp,
			  _ll_free_gmp);

  _ll_initialized_bignum = 1;

  ll_set_g(SbignumC0S, ll_make_bignum_(0));
  ll_set_g(SbignumC1S, ll_make_bignum_(1));

  ll_set_g(SbignumCminS, ll_make_bignum_(ll_MIN_fixnum));
  ll_set_g(SbignumCmaxS, ll_make_bignum_(ll_MAX_fixnum));

  return 0;
}

