#include "ll.h"


/************************************************************************/

#define N ll_THIS->_numerator
#define D ll_THIS->_denominator

/************************************************************************/


ll_v ll_make_ratnum(ll_v n, ll_v d)
{
  return ll_call(ll_o(make), _3(ll_type(ratnum), n, d));
}


ll_define_primitive(ratnum, initialize, _3(r, n, d), _1(no_side_effect, "#t"))
{
  ll_v gcd;

  N = ll_ARG_1;
  D = ll_ARG_2;
  if ( ll_negativeQ(D) ) {
    N = ll__NEG(N);
    D = ll__NEG(D);
  }
  if ( ll_zeroQ(D) ) {
    ll_return(_ll_error(ll_re(divide_by_zero),
			2,
			ll_s(numerator), ll_ARG_1,
			ll_s(denominator), ll_ARG_2));
  }
  gcd = ll_call(ll_o(gcd), _2(N, D));
  N = ll__DIV(N, gcd);
  D = ll__DIV(D, gcd);
  if ( ll_oneQ(D) ) {
    ll_return(N);
  }
  ll_return(ll_SELF);
}
ll_define_primitive_end


ll_define_primitive(ratnum, numerator, _1(q), _1(no_side_effect, "#t"))
{
  ll_return(N);
}
ll_define_primitive_end


ll_define_primitive(ratnum, denominator, _1(q), _1(no_side_effect, "#t"))
{
  ll_return(D);
}
ll_define_primitive_end


/************************************************************************/


ll_define_primitive(ratnum, oddQ, _1(x), _1(no_side_effect, "#t"))
{
  ll_return(ll_f);
}
ll_define_primitive_end


ll_define_primitive(ratnum, evenQ, _1(x), _1(no_side_effect, "#t"))
{
  ll_return(ll_f);
}
ll_define_primitive_end


/************************************************************************/


ll_define_primitive(ratnum, quotient, _2(n1, n2), _1(no_side_effect, "#t"))
{
}
ll_define_primitive_end


ll_define_primitive(ratnum, remainder, _2(n1, n2), _1(no_side_effect, "#t"))
{
}
ll_define_primitive_end


ll_define_primitive(ratnum, modulo, _2(n1, n2), _1(no_side_effect, "#t"))
{
}
ll_define_primitive_end


/************************************************************************/


ll_define_primitive(ratnum, exact__inexact, _1(z), _1(no_side_effect, "#t"))
{
  ll_v n = ll_call(ll_o(exact__inexact), _1(ll_THIS->_numerator));
  ll_v d = ll_call(ll_o(exact__inexact), _1(ll_THIS->_denominator));
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
  ll_call(ll_o(write), _2(N, port));
  ll_call(ll_o(write), _2(ll_make_char('/'), port));
  ll_call(ll_o(write), _2(D, port));
  ll_return(str);
}

ll_define_primitive_end


/************************************************************************/
/* numeric and relational subprims */


ll_define_primitive(ratnum, abs, _1(x), _1(no_side_effect, "#t"))
{
  if ( ll_negativeQ(N) ) {
    ll_return(ll_make_ratnum(ll_abs(N), D));
  } else {
    ll_return(ll_SELF);
  }
}
ll_define_primitive_end


ll_define_primitive(ratnum, _NEG, _1(x), _1(no_side_effect, "#t"))
{
  ll_return(ll_make_ratnum(ll__NEG(N), D));
}
ll_define_primitive_end


ll_define_primitive(ratnum, _ADD, _2(x, y), _1(no_side_effect, "#t"))
{
  if ( ll_ISA_fixnum(ll_ARG_1) ) {
    ll_return(ll_make_ratnum(ll__ADD(N, ll__MUL(ll_ARG_1, D)), D));
  } else {
  }
}
ll_define_primitive_end


ll_define_primitive(ratnum, _SUB, _2(x, y), _1(no_side_effect, "#t"))
{
  if ( ll_ISA_fixnum(ll_ARG_1) ) {
    ll_return(ll_make_ratnum(ll__SUB(N, ll__MUL(ll_ARG_1, D)), D));
  } else {
  }
}
ll_define_primitive_end


ll_define_primitive(ratnum, _MUL, _2(x, y), _1(no_side_effect, "#t"))
{
  ll_return(ll_make_ratnum(ll__MUL(N, ll_numerator(ll_ARG_1)),
			   ll__MUL(D, ll_denominator(ll_ARG_1))));
}
ll_define_primitive_end


ll_define_primitive(ratnum, _DIV, _2(x, y), _1(no_side_effect, "#t"))
{
  ll_return(ll_make_ratnum(ll__MUL(N, ll_denominator(ll_ARG_1)),
			   ll__MUL(D, ll_numerator(ll_ARG_1))));
}
ll_define_primitive_end


/************************************************************************/

#undef N
#undef D


