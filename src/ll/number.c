#include "ll.h"
#include "call_int.h"


/************************************************************************/


ll_define_primitive(object, ADD, __0(x), _1(no_side_effect,"#t"))
{
  ll_v x;

  if ( ll_ARGC == 0 ) {
    x = ll_make_fixnum(0);
  } else {
    int i;

    x = ll_ARG_0;
    for ( i = 1; i < ll_ARGC; i ++ ) {
      x = ll_call(ll_o(_ADD), _2(x, ll_ARGV[i]));
    }
  }
  ll_return(x);
}
ll_define_primitive_end


ll_define_primitive(object, MUL, __0(x), _1(no_side_effect,"#t"))
{
  ll_v x;

  if ( ll_ARGC == 0 ) {
    x = ll_make_fixnum(1);
  } else {
    int i;

    x = ll_ARG_0;
    for ( i = 1; i < ll_ARGC; i ++ ) {
      x = ll_call(ll_o(_MUL), _2(x, ll_ARGV[i]));
    }
  }
  ll_return(x);
}
ll_define_primitive_end


ll_define_primitive(number, SUB, __1(x,y), _1(no_side_effect,"#t"))
{

  if ( ll_ARGC == 1 ) {
    ll_call_tail(ll_o(_NEG), _1(ll_SELF));
  } else {
    int i;
    ll_v x = ll_ARG_1;
    for ( i = 2; i < ll_ARGC; i ++ ) {
      x = ll_call(ll_o(_ADD), _2(x, ll_ARGV[i]));
    }
    ll_call_tail(ll_o(_SUB), _2(ll_SELF, x));
  }
}
ll_define_primitive_end


ll_define_primitive(number, DIV, __1(x,y), _1(no_side_effect,"#t"))
{

  if ( ll_ARGC == 1 ) {
    ll_call_tail(ll_o(_DIV), _2(ll_make_fixnum(1), ll_SELF));
  } else {
    int i;
    ll_v x = ll_ARG_1;
    for ( i = 2; i < ll_ARGC; i ++ ) {
      x = ll_call(ll_o(_MUL), _2(x, ll_ARGV[i]));
    }
    ll_call_tail(ll_o(_DIV), _2(ll_SELF, x));
  }
}
ll_define_primitive_end


/************************************************************************/


ll_define_primitive(object, gcd, __0(n), _1(no_side_effect,"#t"))
{
  if ( ll_ARGC == 0 ) {
    ll_return(ll_make_fixnum(0));
  } else {
    /* From Knuth, v. 2, p. 341 */
    int k = ll_ARGC - 1;
    ll_v d = ll_ARGV[k];
    
    while ( k >= 0 ) {
      k --;
      d = ll_call(ll_o(_gcd), _2(ll_ARGV[k], d));
    }
    ll_return(d);
  }
}
ll_define_primitive_end


ll_define_primitive(object, lcm, __0(n), _1(no_side_effect,"#t"))
{
  if ( ll_ARGC == 0 ) {
    ll_return(ll_make_fixnum(1));
  } else {
    int k = ll_ARGC - 1;
    ll_v d = ll_ARGV[k];
    
    while ( k >= 0 ) {
      k --;
      d = ll_call(ll_o(_lcm), _2(ll_ARGV[k], d));
    }
    ll_return(d);
  }
}
ll_define_primitive_end

/************************************************************************/


ll_define_primitive(real, real_part, _1(z), _1(no_side_effect,"#t"))
{
  ll_return(ll_SELF);
}
ll_define_primitive_end


ll_define_primitive(real, imag_part, _1(z), _1(no_side_effect,"#t"))
{
  ll_return(ll_make_fixnum(0));
}
ll_define_primitive_end


ll_define_primitive(real, magnitude, _1(z), _1(no_side_effect,"#t"))
{
  __ll_call_tailv(ll_o(abs), 1);
}
ll_define_primitive_end


ll_define_primitive(real, angle, _1(z), _1(no_side_effect,"#t"))
{
  ll_return(ll_make_fixnum(0));
}
ll_define_primitive_end


/************************************************************************/


#define ROP(LT) \
ll_define_primitive(number, LT, __2(x, y, z), _1(no_side_effect,"#t")) \
{ \
  int i; \
  for ( i = 1; i < ll_ARGC; i ++ ) { \
    ll_v x = ll_call(ll_o(_##LT), _2(ll_ARGV[i - 1], ll_ARGV[i])); \
    if ( ll_EQ(x, ll_f) ) \
      ll_return(ll_f); \
  } \
  ll_return(ll_t); \
} \
ll_define_primitive_end

ROP(LT)
ROP(GT)
ROP(LE)
ROP(GE)
ROP(EQ)

#undef ROP


/************************************************************************/


ll_define_primitive(number, max, __2(z1,z2,z3), _1(no_side_effect,"#t"))
{
  ll_v x = ll_SELF;
  int i;

  for ( i = 1; i < ll_ARGC; i ++ ) {
    if ( ll_unbox_boolean(ll_call(ll_o(_GT), _2(ll_ARGV[i], x))) ) {
      x = ll_ARGV[i];
    }
  }

  ll_return(x);
}
ll_define_primitive_end


ll_define_primitive(number, min, __2(z1,z2,z3), _1(no_side_effect,"#t"))
{
  ll_v x = ll_SELF;
  int i;

  for ( i = 1; i < ll_ARGC; i ++ ) {
    if ( ll_unbox_boolean(ll_call(ll_o(_LT), _2(ll_ARGV[i], x))) ) {
      x = ll_ARGV[i];
    }
  }

  ll_return(x);
}
ll_define_primitive_end


/************************************************************************/


int ll_negativeQ(ll_v x)
{
  if ( ll_ISA_fixnum(x) ) {
    return ll_UNBOX_fixnum(x) < 0;
  } else {
    return ll_unbox_boolean(ll_call(ll_o(negativeQ), _1(x)));
  }
}


int ll_zeroQ(ll_v x)
{
  if ( ll_ISA_fixnum(x) ) {
    return ll_UNBOX_fixnum(x) == 0;
  } else {
    return ll_unbox_boolean(ll_call(ll_o(zeroQ), _1(x)));
  }
}


int ll_oneQ(ll_v x)
{
  if ( ll_ISA_fixnum(x) ) {
    return ll_UNBOX_fixnum(x) == 1;
  } else {
    return ll_unbox_boolean(ll_call(ll_o(oneQ), _1(x)));
  }
}


int ll_positiveQ(ll_v x)
{
  if ( ll_ISA_fixnum(x) ) {
    return ll_UNBOX_fixnum(x) > 0;
  } else {
    return ll_unbox_boolean(ll_call(ll_o(positiveQ), _1(x)));
  }
}


ll_v ll_abs(ll_v x)
{
  return ll_call(ll_o(abs), _1(x));
}


ll_v ll_numerator(ll_v x)
{
  return ll_call(ll_o(numerator), _1(x));
}


ll_v ll_denominator(ll_v x)
{
  return ll_call(ll_o(denominator), _1(x));
}


#define BOP(N,O) ll_v ll__##N(ll_v x, ll_v y) { return(ll_call(ll_o(_##N), _2(x, y))); }
#define UOP(N,O) ll_v ll__##N(ll_v x) { return(ll_call(ll_o(_##N), _1(x))); }

#include "cops.h"


