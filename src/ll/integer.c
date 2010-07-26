#include "ll.h"

/************************************************************************/

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


ll_define_primitive(integer, inexact__exact, _1(z), _1(no_side_effect,"#t"))
{
  ll_return(ll_SELF);
}
ll_define_primitive_end

/************************************************************************/


ll_INIT(integer, 254, "integer constants")
{
  return 0;
}

