#include "ll.h"
#include <math.h>
#include "ll/floatcfg.h"
#include <string.h> /* strchr, strcat */

/************************************************************************/

#if ! _ll_flonum_imm_supported
ll_v ll_make_flonum(double x)
{
  ll_v *obj = ll_malloc(sizeof(ll_v) + sizeof(x));
  *obj = ll_type(flonum);
  *(double*) (obj + 1) = x;
  return ll_BOX_ref(obj);
}
#endif

double ll_unbox_flonum(ll_v x)
{
  while ( ! ll_ISA_flonum(x) ) {
    x = _ll_typecheck_error(ll_type(flonum), x);
  }
  return ll_UNBOX_flonum(x);
}

#define _ll_NUM flonum
#define _ll_NUM_BOX(X)ll_make_flonum(X)
#define _ll_NUM_UNBOX(X)ll_unbox_flonum(X)

#include "num.c"

ll_v ll_coerce_flonum(ll_v x)
{
  if ( ll_ISA_flonum(x) ) {
    return x;
  }
  else if ( ll_ISA_fixnum(x) ) {
    return ll_make_flonum(ll_UNBOX_fixnum(x));
  }
  else if ( ll_ISA_ratnum(x) || ll_ISA_bignum(x) ) {
    return ll_call(ll_o(exact__inexact), _1(x));
  }
  else {
    return _ll_typecheck(ll_type(flonum), &x);
  }
}

/**************************************************************************/

ll_define_primitive(flonum, floor, _1(x), _1(no_side_effect,"#t"))
{
  ll_return(ll_make_integer_d(floor(ll_UNBOX_flonum(ll_SELF))));
}
ll_define_primitive_end

ll_define_primitive(flonum, ceiling, _1(x), _1(no_side_effect,"#t"))
{
  ll_return(ll_make_integer_d(ceil(ll_UNBOX_flonum(ll_SELF))));
}
ll_define_primitive_end

ll_define_primitive(flonum, truncate, _1(x), _1(no_side_effect,"#t"))
{
  double x = ll_UNBOX_flonum(ll_SELF);
  if ( x > 0 ) {
    ll_return(ll_make_integer_d(floor(ll_UNBOX_flonum(ll_SELF))));
  } else {
    ll_return(ll_make_integer_d(ceil(ll_UNBOX_flonum(ll_SELF))));
  }
  ll_return(ll_SELF);
}
ll_define_primitive_end

#include "fenv.h"

ll_define_primitive(flonum, round, _1(x), _1(no_side_effect,"#t"))
{
#if 1
  int old_round_mode = fegetround();
#endif
  double x = round(ll_UNBOX_flonum(ll_SELF));
#if 1
  fesetround(FE_TONEAREST);
#endif
  x = round(x);
#if 1
  fesetround(old_round_mode);
#endif
  ll_return(ll_make_flonum(x));
}
ll_define_primitive_end

/************************************************************************/

ll_define_primitive(flonum, exact__inexact, _1(z), _1(no_side_effect,"#t"))
{
  ll_return(ll_SELF);
}
ll_define_primitive_end

ll_define_primitive(flonum, inexact__exact, _1(z), _1(no_side_effect,"#t"))
{
  ll_return(ll_SELF);
}
ll_define_primitive_end

/************************************************************************/

ll_define_primitive(flonum, number__string, __1(z,radix), _1(no_side_effect,"#t"))
{
  char buf[32];
  sprintf(buf, "%." ll_STRINGTIZE(ll_FLO_DIGITS) "g", (double) ll_UNBOX_flonum(ll_SELF));
  if ( ! (strchr(buf, 'e') || strchr(buf, '.')) ) {
    strcat(buf, ".0");
  }
  ll_return(ll_make_copy_string(buf, (size_t) -1)); 
}
ll_define_primitive_end

/************************************************************************/

#define BOP(N,O) \
ll_define_primitive(flonum, _##N, _2(self, x), _1(no_side_effect,"#t")) \
{									\
  if ( ll_ISA_flonum(ll_ARG_1) ) {					\
    ll_return(ll_make_flonum(ll_UNBOX_flonum(ll_SELF) O ll_UNBOX_flonum(ll_ARG_1))); \
  } else if ( ll_ISA_fixnum(ll_ARG_1) ) {				\
    ll_return(ll_make_flonum(ll_UNBOX_flonum(ll_SELF) O ll_UNBOX_fixnum(ll_ARG_1))); \
  } else if ( ll_ISA_ratnum(ll_ARG_1) || ll_ISA_bignum(ll_ARG_1) ) {	\
    ll_return(ll_make_flonum(ll_UNBOX_flonum(ll_SELF) O ll_UNBOX_flonum(ll_coerce_flonum(ll_ARG_1)))); \
  } else {								\
    ll_return(_ll_typecheck(ll_type(number), &ll_ARG_1));		\
  }									\
}									\
ll_define_primitive_end

#define UOP(N,O) \
ll_define_primitive(flonum, _##N, _1(self), _1(no_side_effect,"#t")) \
{ \
  ll_return(ll_make_flonum( O ll_UNBOX_flonum(ll_SELF))); \
} \
ll_define_primitive_end

#define ROP(N,OP) \
ll_define_primitive(flonum, _##N, _2(self, x), _1(no_side_effect,"#t")) \
{									\
  if ( ll_ISA_flonum(ll_ARG_1) ) {					\
    ll_return(ll_make_boolean(ll_UNBOX_flonum(ll_SELF) OP ll_UNBOX_flonum(ll_ARG_1))); \
  } else if ( ll_ISA_fixnum(ll_ARG_1) ) {				\
    ll_return(ll_make_boolean(ll_UNBOX_flonum(ll_SELF) OP ll_UNBOX_fixnum(ll_ARG_1))); \
  } else if ( ll_ISA_ratnum(ll_ARG_1) || ll_ISA_bignum(ll_ARG_1) ) {	\
    ll_return(ll_make_boolean(ll_UNBOX_flonum(ll_SELF) OP ll_UNBOX_flonum(ll_coerce_flonum(ll_ARG_1)))); \
  } else {								\
    ll_return(_ll_typecheck(ll_type(number), &ll_ARG_1));		\
  }									\
}									\
ll_define_primitive_end

#include "cops.h"

/**************************************************************************/

