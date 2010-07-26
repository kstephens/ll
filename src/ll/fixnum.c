#include "ll.h"


/************************************************************************/


long ll_unbox_fixnum(ll_v x)
{
  while ( ! ll_ISA_fixnum(x) ) {
    x = _ll_typecheck_error(ll_type(fixnum), x);
  }
  return ll_UNBOX_fixnum(x);
}

#define _ll_NUM fixnum
#define _ll_NUM_BOX(X)ll_make_fixnum(X)
#define _ll_NUM_UNBOX(X)ll_unbox_fixnum(X)

#include "num.c"


/************************************************************************/


ll_define_primitive(fixnum, oddQ, _1(x), _1(no_side_effect,"#t"))
{
  ll_return(ll_make_boolean(ll_UNBOX_fixnum(ll_SELF) % 2) != 0);
}
ll_define_primitive_end


ll_define_primitive(fixnum, evenQ, _1(x), _1(no_side_effect,"#t"))
{
  ll_return(ll_make_boolean((ll_UNBOX_fixnum(ll_SELF) % 2) == 0));
}
ll_define_primitive_end


/************************************************************************/


ll_define_primitive(fixnum, quotient, _2(n1, n2), _1(no_side_effect,"#t"))
{
  ll_return(ll_make_fixnum(ll_UNBOX_fixnum(ll_SELF) / ll_unbox_fixnum(ll_ARG_1)));
}
ll_define_primitive_end


ll_define_primitive(fixnum, remainder, _2(n1, n2), _1(no_side_effect,"#t"))
{
  ll_return(ll_make_fixnum(ll_UNBOX_fixnum(ll_SELF) % ll_unbox_fixnum(ll_ARG_1)));
}
ll_define_primitive_end


ll_define_primitive(fixnum, modulo, _2(n1, n2), _1(no_side_effect,"#t"))
{
  long n1, n2, m;
  n1 = ll_UNBOX_fixnum(ll_ARG_0);
  n2 = ll_unbox_fixnum(ll_ARG_1);

  m = n1 % n2;

  if ( ((n2 > 0) ^ (m < 0)) == 0 ) {
      m = n2 + m;
  }

  ll_return(ll_make_fixnum(m));
}
ll_define_primitive_end


/************************************************************************/


ll_define_primitive(fixnum, _gcd, _2(n1, n2), _1(no_side_effect,"#t"))
{
  long u, v, r;

  /* Euclid from Knuth, V2, P.337 */

  u = ll_UNBOX_fixnum(ll_ARG_0);
  if ( u < 0 ) u = - u;
  v = ll_unbox_fixnum(ll_ARG_1);
  if ( v < 0 ) v = - v;
  
  while ( v ) {
    r = u % v;
    u = v;
    v = r;
  }

  ll_return(ll_make_fixnum(u));
}
ll_define_primitive_end


/************************************************************************/


ll_define_primitive(fixnum, rationalize, _2(n1, n2), _1(no_side_effect,"#t"))
{
  ll_return(ll_make_flonum((double) ll_UNBOX_fixnum(ll_SELF) / (double) ll_unbox_fixnum(ll_ARG_1)));
}
ll_define_primitive_end


/************************************************************************/


ll_define_primitive(fixnum, exact__inexact, _1(z), _1(no_side_effect,"#t"))
{
  ll_return(ll_make_flonum(ll_UNBOX_fixnum(ll_SELF)));
}
ll_define_primitive_end


/************************************************************************/


ll_define_primitive(fixnum, number__string, __1(z,radix), _1(no_side_effect,"#t"))
{
  char buf[sizeof(ll_v) * 8 + 2], *s = buf + sizeof(buf);
  int radix = 10;
  long x = ll_UNBOX_fixnum(ll_SELF);
  int neg;

  if ( (neg = x < 0) ) {
    x = - x;
  }

  if ( ll_ARGC >= 2 ) {
    radix = ll_unbox_fixnum(_ll_rangecheck(ll_s(radix), &ll_ARG_1, 2, 36));
  }

  *(-- s) = '\0';
  do {
    *(-- s) = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"[x % radix];
    x /= radix;
  } while ( x );

  if ( neg ) {
    *(-- s) = '-';
  }

  ll_return(ll_make_copy_string(s, (size_t) -1));
}

ll_define_primitive_end


/************************************************************************/
/* numeric and relational subprims */


#define BOP(N,O) \
ll_define_primitive(fixnum, _##N, _2(n1, n2), _1(no_side_effect,"#t")) \
{ \
  if ( ll_ISA_fixnum(ll_ARG_1) ) { \
    ll_return(ll_make_fixnum(ll_UNBOX_fixnum(ll_SELF) O ll_UNBOX_fixnum(ll_ARG_1))); \
  } else if ( ll_ISA_flonum(ll_ARG_1) ) { \
    ll_return(ll_make_flonum(ll_UNBOX_fixnum(ll_SELF) O ll_UNBOX_flonum(ll_ARG_1))); \
  } else { \
    ll_return(_ll_typecheck(ll_type(number), &ll_ARG_1)); \
  } \
} \
ll_define_primitive_end


#define UOP(N,O) \
ll_define_primitive(fixnum, _##N, _1(n), _1(no_side_effect,"#t")) \
{ \
  ll_return(ll_make_fixnum( O ll_UNBOX_fixnum(ll_SELF))); \
} \
ll_define_primitive_end


#define ROP(N,OP) \
ll_define_primitive(fixnum, _##N, _2(n1, n2), _1(no_side_effect,"#t")) \
{ \
  if ( ll_ISA_fixnum(ll_ARG_1) ) { \
    ll_return(ll_make_boolean(ll_UNBOX_fixnum(ll_SELF) OP ll_UNBOX_fixnum(ll_ARG_1))); \
  } else if ( ll_ISA_flonum(ll_ARG_1) ) { \
    ll_return(ll_make_boolean(ll_UNBOX_fixnum(ll_SELF) OP ll_UNBOX_flonum(ll_ARG_1))); \
  } else { \
    ll_return(_ll_typecheck(ll_type(number), &ll_ARG_1)); \
  } \
} \
ll_define_primitive_end


#include "cops.h"


/************************************************************************/
/* bitwise operations */


#define BOP(N,O) \
ll_define_primitive(fixnum, _##N, _2(n1, n2), _1(no_side_effect,"#t")) \
{ \
  _ll_typecheck(ll_type(fixnum), &ll_ARG_1); \
  ll_return(ll_make_fixnum(ll_UNBOX_fixnum(ll_SELF) O ll_UNBOX_fixnum(ll_ARG_1))); \
} \
ll_define_primitive_end


#define UOP(N,O) \
ll_define_primitive(fixnum, _##N, _1(n), _1(no_side_effect,"#t")) \
{ \
  ll_return(ll_make_fixnum( O ll_UNBOX_fixnum(ll_SELF))); \
} \
ll_define_primitive_end


#define INT_OPS


#include "cops.h"


/************************************************************************/


ll_INIT(fixnum, 253, "fixnum constants")
{
  ll_set_g(SfixnumCminS, ll_BOX_fixnum(ll_MIN_fixnum));
  ll_set_g(SfixnumCmaxS, ll_BOX_fixnum(ll_MAX_fixnum));

  return 0;
}

