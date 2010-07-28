#include "ll.h"


/************************************************************************/

#define Xr ll_THIS
#define Xn ll_THIS->_numerator
#define Xd ll_THIS->_denominator

#define _r(Y) ll_THIS_ISA(ratnum, Y)
#define Yr _r(Y)
#define Yn Yr->_numerator
#define Yd Yr->_denominator


/************************************************************************/


ll_v ll_make_ratnum_(ll_v n, ll_v d)
{
  n = ll_call(ll_o(make), _3(ll_type(ratnum), n, d));
  return n;
}


ll_v ll_make_ratnum(ll_v n, ll_v d)
{
  n = ll_make_ratnum_(n, d);
  n = ll_call(ll_o(_unify), _1(n));
  return n;
}


ll_v ll_coerce_ratnum(ll_v n)
{
  if ( ll_ISA_ratnum(n) ) {
    return n;
  }
  else if ( ll_ISA_fixnum(n) || ll_ISA_bignum(n) ) {
    return ll_make_ratnum_(n, ll_BOX_fixnum(1));
  }
  else {
    ll_abort("ll_coerce_ratnum()");
    return ll_f;
  }
}


ll_define_primitive(ratnum, initialize, _3(r, n, d), _1(no_side_effect, "#t"))
{
  ll_v gcd;

  // ll_format(ll_f, "  ratnum: (~S / ~S) ...\n", 2, ll_ARG_1, ll_ARG_2);

  Xn = ll_ARG_1;
  Xd = ll_ARG_2;
  if ( ll_negativeQ(Xd) ) {
    Xn = ll__NEG(Xn);
    Xd = ll__NEG(Xd);
  }
  if ( ll_zeroQ(Xd) ) {
    ll_return(_ll_error(ll_re(divide_by_zero),
			2,
			ll_s(numerator), ll_ARG_1,
			ll_s(denominator), ll_ARG_2));
  }
  gcd = ll_call(ll_o(gcd), _2(Xn, Xd));
  // ll_format(ll_f, "  ratnum: gcd(~S, ~S) => ~S", 3, Xn, Xd, gcd);
  Xn = ll__DIV(Xn, gcd);
  Xd = ll__DIV(Xd, gcd);
  ll_return(ll_SELF);
}
ll_define_primitive_end



ll_define_primitive(ratnum, _unify, _1(r), _1(no_side_effect, "#t"))
{
  if ( ll_oneQ(Xd) ) {
    ll_return(Xn);
  }
  ll_return(ll_SELF);
}
ll_define_primitive_end



ll_define_primitive(ratnum, numerator, _1(q), _1(no_side_effect, "#t"))
{
  ll_return(Xn);
}
ll_define_primitive_end


ll_define_primitive(ratnum, denominator, _1(q), _1(no_side_effect, "#t"))
{
  ll_return(Xd);
}
ll_define_primitive_end


/************************************************************************/


ll_define_primitive(ratnum, negativeQ, _1(x), _1(no_side_effect, "#t"))
{
  ll_return(ll_negativeQ(Xn));
}
ll_define_primitive_end

ll_define_primitive(ratnum, zeroQ, _1(x), _1(no_side_effect, "#t"))
{
  ll_return(ll_zeroQ(Xn));
}
ll_define_primitive_end

ll_define_primitive(ratnum, positiveQ, _1(x), _1(no_side_effect, "#t"))
{
  ll_return(ll_negativeQ(Xn));
}
ll_define_primitive_end


ll_define_primitive(ratnum, oneQ, _1(x), _1(no_side_effect, "#t"))
{
  ll_return(ll_oneQ(Xn));
}
ll_define_primitive_end


/************************************************************************/


ll_define_primitive(ratnum, exact__inexact, _1(z), _1(no_side_effect, "#t"))
{
  ll_v n = ll_call(ll_o(exact__inexact), _1(Xn));
  ll_v d = ll_call(ll_o(exact__inexact), _1(Xd));
  ll_return(ll_make_flonum(ll_unbox_flonum(n) / ll_unbox_flonum(d)));
}
ll_define_primitive_end


/************************************************************************/


ll_define_primitive(ratnum, number__string, __1(z, radix), _1(no_side_effect, "#t"))
{
  ll_v str = ll_make_string(0, 0);
  ll_v port = ll_call(ll_o(make), _2(ll_type(output_port), str));
  ll_v r = ll_ARGC >= 2 ? ll_ARG_1 : ll_BOX_fixnum(10);
  /* FIXME: handle radix! */
  (void) r;
  ll_call(ll_o(write), _2(Xn, port));
  ll_call(ll_o(write), _2(ll_make_char('/'), port));
  ll_call(ll_o(write), _2(Xd, port));
  ll_return(str);
}

ll_define_primitive_end


/************************************************************************/
/* numeric and relational subprims */


ll_define_primitive(ratnum, abs, _1(x), _1(no_side_effect, "#t"))
{
  if ( ll_negativeQ(Xn) ) {
    ll_return(ll_make_ratnum(ll_abs(Xn), Xd));
  } else {
    ll_return(ll_SELF);
  }
}
ll_define_primitive_end


ll_define_primitive(ratnum, _NEG, _1(x), _1(no_side_effect, "#t"))
{
  ll_return(ll_make_ratnum(ll__NEG(Xn), Xd));
}
ll_define_primitive_end


#define Y ll_ARG_1

ll_define_primitive(ratnum, _ADD, _2(x, y), _1(no_side_effect, "#t"))
{
  if ( ll_ISA_fixnum(Y) ) {
    ll_return(ll_make_ratnum(ll__ADD(Xn, ll__MUL(Y, Xd)), Xd));
  }
  else if ( ll_ISA_flonum(Y) ) {
    ll_return(ll__ADD(ll_coerce_flonum(ll_SELF), Y));
  } 
  else if ( ll_ISA_ratnum(Y) ) {
    ll_return(ll_make_ratnum(ll__ADD(ll__MUL(Xn, Yd), ll__MUL(Yn, Xd)), 
			     ll__MUL(Xd, Yd))); 
  } 
  else {
    ll_return(_ll_typecheck(ll_type(number), &ll_ARG_1));
  }
}
ll_define_primitive_end


ll_define_primitive(ratnum, _SUB, _2(x, y), _1(no_side_effect, "#t"))
{
  if ( ll_ISA_fixnum(Y) ) {
    ll_return(ll_make_ratnum(ll__SUB(Xn, ll__MUL(Y, Xd)), Xd));
  } 
  else if ( ll_ISA_flonum(Y) ) {
    ll_return(ll__SUB(ll_coerce_flonum(ll_SELF), Y));
  } 
  else if ( ll_ISA_ratnum(Y) ) {
    ll_return(ll_make_ratnum(ll__SUB(ll__MUL(Xn, Yd), ll__MUL(Yn, Xd)), 
			     ll__MUL(Xd, Yd)));
  } 
  else {
    ll_return(_ll_typecheck(ll_type(number), &ll_ARG_1));
  }
}
ll_define_primitive_end


ll_define_primitive(ratnum, _MUL, _2(x, y), _1(no_side_effect, "#t"))
{
  if ( ll_ISA_fixnum(Y) ) {
    ll_return(ll_make_ratnum(ll__MUL(Xn, Y), Xd));
  } 
  else if ( ll_ISA_flonum(Y) ) {
    ll_return(ll__MUL(ll_coerce_flonum(ll_SELF), Y));
  }
  else {
    ll_return(ll_make_ratnum(ll__MUL(Xn, ll_numerator(Y)),
			     ll__MUL(Xd, ll_denominator(Y))));
  }
}
ll_define_primitive_end


ll_define_primitive(ratnum, _DIV, _2(x, y), _1(no_side_effect, "#t"))
{
  if ( ll_ISA_fixnum(Y) ) {
    ll_return(ll_make_ratnum(Xn, ll__MUL(Xd, Y)));
  } 
  else if ( ll_ISA_flonum(Y) ) {
    ll_return(ll__DIV(ll_coerce_flonum(ll_SELF), Y));
  }
  else {
    ll_return(ll_make_ratnum(ll__MUL(Xn, ll_denominator(Y)),
			     ll__MUL(Xd, ll_numerator(Y))));
  }
}
ll_define_primitive_end



ll_define_primitive(ratnum, equalQ, _2(n1, n2), _1(no_side_effect,"#t")) 
{
  if ( ll_ISA_ratnum(Y) ) {
    ll_return(ll_make_boolean(ll_equalQ(Xn, Yn) &&
			      ll_equalQ(Xd, Yd)));
  }
  ll_return(ll_f);
}
ll_define_primitive_end


ll_define_primitive(ratnum, eqvQ, _2(n1, n2), _1(no_side_effect,"#t")) 
{
  if ( ll_ISA_ratnum(Y) ) {
    ll_return(ll_make_boolean(ll_equalQ(Xn, Yn) &&
			      ll_equalQ(Xd, Yd)));
  }
  ll_return(ll_f);
}
ll_define_primitive_end


#define BOP(N,OP)
#define UOP(N,OP)
#define ROP(NOP,OP) \
ll_define_primitive(ratnum, _##NOP, _2(n1, n2), _1(no_side_effect,"#t")) \
{									\
  if ( ll_ISA_flonum(ll_ARG_1) ) {					\
    ll_return(ll_make_boolean(ll_coerce_flonum(ll_SELF) OP ll_UNBOX_flonum(ll_ARG_1))); \
  } else {								\
    ll_v xn, yn;							\
    ll_ARG_1 = ll_coerce_ratnum(ll_ARG_1);				\
    if ( ll_EQ(ll_THIS->_denominator, _r(ll_ARG_1)->_denominator) ) {	\
      xn = ll_THIS->_numerator;						\
      yn = _r(ll_ARG_1)->_numerator;					\
    } else {								\
      xn = ll__MUL(ll_THIS->_numerator, _r(ll_ARG_1)->_denominator);	\
      yn = ll__MUL(  _r(Y)->_numerator,      ll_THIS->_denominator);	\
    }									\
    ll_return(ll__##NOP(xn, yn));					\
  }									\
}									\
ll_define_primitive_end

#include "cops.h"

/************************************************************************/

#undef N
#undef D


