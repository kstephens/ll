#ifndef __rcs_id__
#ifndef __rcs_id_ll_flonum_c__
#define __rcs_id_ll_flonum_c__
static const char __rcs_id_ll_flonum_c[] = "$Id: flonum.c,v 1.12 2008/01/06 18:36:33 stephens Exp $";
#endif
#endif /* __rcs_id__ */

#include "ll.h"
#include <math.h>
#include "ll/floatcfg.h"
#include <string.h> /* strchr, strcat */


/************************************************************************/

float ll_unbox_flonum(ll_v x)
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
  ll_return(ll_make_fixnum(floor(ll_UNBOX_flonum(ll_SELF))));
}
ll_define_primitive_end


ll_define_primitive(flonum, ceiling, _1(x), _1(no_side_effect,"#t"))
{
  ll_return(ll_make_fixnum(ceil(ll_UNBOX_flonum(ll_SELF))));
}
ll_define_primitive_end


ll_define_primitive(flonum, truncate, _1(x), _1(no_side_effect,"#t"))
{
  double x = ll_UNBOX_flonum(ll_SELF);
  if ( x > 0 ) {
    ll_return(ll_make_fixnum(floor(ll_UNBOX_flonum(ll_SELF))));
  } else {
    ll_return(ll_make_fixnum(ceil(ll_UNBOX_flonum(ll_SELF))));
  }
  ll_return(ll_SELF);
}
ll_define_primitive_end


ll_define_primitive(flonum, round, _1(x), _1(no_side_effect,"#t"))
{
  /* IMPLEMENT: CHECK FOR X.5 */
  ll_return(ll_make_fixnum(floor(ll_UNBOX_flonum(ll_SELF) + 0.5)));
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
{ \
  if ( ll_ISA_flonum(ll_ARG_1) ) { \
    ll_return(ll_make_flonum(ll_UNBOX_flonum(ll_SELF) O ll_UNBOX_flonum(ll_ARG_1))); \
  } else if ( ll_ISA_fixnum(ll_ARG_1) ) { \
    ll_return(ll_make_flonum(ll_UNBOX_flonum(ll_SELF) O ll_UNBOX_fixnum(ll_ARG_1))); \
  } else if ( ll_ISA_ratnum(ll_ARG_1) ) { \
    ll_return(ll_make_flonum(ll_UNBOX_flonum(ll_SELF) O ll_UNBOX_flonum(ll_coerce_flonum(ll_ARG_1)))); \
  } else { \
    ll_return(_ll_typecheck(ll_type(number), &ll_ARG_1)); \
  } \
} \
ll_define_primitive_end


#define UOP(N,O) \
ll_define_primitive(flonum, _##N, _1(self), _1(no_side_effect,"#t")) \
{ \
  ll_return(ll_make_flonum( O ll_UNBOX_flonum(ll_SELF))); \
} \
ll_define_primitive_end


#define ROP(N,OP) \
ll_define_primitive(flonum, _##N, _2(self, x), _1(no_side_effect,"#t")) \
{ \
  if ( ll_ISA_flonum(ll_ARG_1) ) { \
    ll_return(ll_make_boolean(ll_UNBOX_flonum(ll_SELF) OP ll_UNBOX_flonum(ll_ARG_1))); \
  } else if ( ll_ISA_fixnum(ll_ARG_1) ) { \
    ll_return(ll_make_boolean(ll_UNBOX_flonum(ll_SELF) OP ll_UNBOX_fixnum(ll_ARG_1))); \
  } else if ( ll_ISA_ratnum(ll_ARG_1) ) { \
    ll_return(ll_make_boolean(ll_UNBOX_flonum(ll_SELF) OP ll_UNBOX_flonum(ll_coerce_flonum(ll_ARG_1)))); \
  } else { \
    ll_return(_ll_typecheck(ll_type(number), &ll_ARG_1)); \
  } \
} \
ll_define_primitive_end

#include "cops.h"


/**************************************************************************/

