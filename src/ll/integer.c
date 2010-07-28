#include "ll.h"


/************************************************************************/


ll_v ll_make_integer(ll_v_word x)
{
  if ( (ll_MIN_fixnum <= x && x <= ll_MAX_fixnum) ) {
    return ll_BOX_fixnum(x);
  }
  return ll_make_bignum_(x);
}


ll_v ll_make_integer_d(double x)
{
  if ( (ll_MIN_fixnum <= x && x <= ll_MAX_fixnum) ) {
    return ll_BOX_fixnum(x);
  }
  return ll_make_bignum_d(x);
}


/************************************************************************/

ll_define_primitive(integer, _gcd, _2(n1, n2), _1(no_side_effect,"#t"))
{
  ll_v u, v, r;

  /* Euclid from Knuth, V2, P.337 */

  u = ll_abs(ll_ARG_0);
  v = ll_abs(ll_ARG_1);
  
  while ( ! ll_zeroQ(v) ) {
    // ll_format(ll_f, "  integer:gcd u=~S v=~S\n", 2, u, v);
    r = ll_modulo(u, v);
    u = v;
    v = r;
  }
  // ll_format(ll_f, "  integer:%gcd(~S, ~S) => ~S\n", 3, ll_ARG_0, ll_ARG_1, u);

  ll_return(u);
}
ll_define_primitive_end


ll_define_primitive(integer, _lcm, _2(n1, n2), _1(no_side_effect,"#t"))
{
  ll_v d = ll_call(ll_o(_gcd), _2(ll_ARG_0, ll_ARG_1));

  ll_call_tail(ll_o(abs), _1(ll_call(ll_o(_MUL), _2(ll_ARG_0, (ll_call(ll_o(_DIV), _2(ll_ARG_1, d)))))));
}
ll_define_primitive_end


/************************************************************************/


ll_define_primitive(integer, numerator, _1(q), _1(no_side_effect,"#t"))
{
  ll_return(ll_SELF);
}
ll_define_primitive_end


ll_define_primitive(integer, denominator, _1(q), _1(no_side_effect,"#t"))
{
  ll_return(ll_BOX_fixnum(1));
}
ll_define_primitive_end


/************************************************************************/


ll_define_primitive(integer, floor, _1(x), _1(no_side_effect,"#t"))
{
  ll_return(ll_SELF);
}
ll_define_primitive_end


ll_define_primitive(integer, ceiling, _1(x), _1(no_side_effect,"#t"))
{
  ll_return(ll_SELF);
}
ll_define_primitive_end


ll_define_primitive(integer, truncate, _1(x), _1(no_side_effect,"#t"))
{
  ll_return(ll_SELF);
}
ll_define_primitive_end


ll_define_primitive(integer, round, _1(x), _1(no_side_effect,"#t"))
{
  ll_return(ll_SELF);
}
ll_define_primitive_end


/************************************************************************/


ll_INIT(integer, 254, "integer constants")
{
  return 0;
}

