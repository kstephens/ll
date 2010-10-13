#include "ll.h"
#include "call_int.h"

#define X ll_SELF
#define Y ll_ARG_1

/************************************************************************/

ll_define_primitive(object, eqQ, _2(x, y), _1(no_side_effect,"#t"))
{
  ll_return(ll_make_boolean(ll_EQ(X, Y)));
}
ll_define_primitive_end

/************************************************************************/

ll_define_primitive(object, eqvQ, _2(x, y), _1(no_side_effect,"#t"))
{
  ll_return(ll_make_boolean(ll_EQ(X, Y)));
}
ll_define_primitive_end

ll_define_primitive(fixnum, eqvQ, _2(x, y), _1(no_side_effect,"#t"))
{
  ll_v t2;

  if ( ll_EQ(ll_ARG_0, ll_ARG_1) ) {
    ll_return(ll_t);
  }
  t2 = ll_TYPE(ll_ARG_1);

  if ( ll_EQ(t2, ll_type(fixnum)) ) {
    ll_return(ll_make_boolean(ll_unbox_fixnum(ll_SELF) == ll_unbox_fixnum(ll_ARG_1)));
  } else if ( ll_EQ(t2, ll_type(flonum)) ) {
    ll_return(ll_make_boolean(ll_unbox_fixnum(ll_SELF) == ll_unbox_flonum(ll_ARG_1)));
  }
  ll_return(ll_f);
}
ll_define_primitive_end

ll_define_primitive(flonum, eqvQ, _2(x, y), _1(no_side_effect,"#t"))
{
  ll_v t2;

  if ( ll_EQ(ll_ARG_0, ll_ARG_1) ) {
    ll_return(ll_t);
  }
  t2 = ll_TYPE(ll_ARG_1);

  if ( ll_EQ(t2, ll_type(flonum)) ) {
    ll_return(ll_make_boolean(ll_unbox_flonum(ll_SELF) == ll_unbox_flonum(ll_ARG_1)));
  } else if ( ll_EQ(t2, ll_type(fixnum)) ) {
    ll_return(ll_make_boolean(ll_unbox_flonum(ll_SELF) == ll_unbox_fixnum(ll_ARG_1)));
  }
  ll_return(ll_f);
}
ll_define_primitive_end


/************************************************************************/

ll_define_primitive(constant, equalQ, _2(x, y), _1(no_side_effect,"#t"))
{
  ll_return(ll_make_boolean(ll_EQ(X, Y)));
}
ll_define_primitive_end

ll_define_primitive(symbol, equalQ, _2(x, y), _1(no_side_effect,"#t"))
{
  ll_return(ll_make_boolean(ll_EQ(X, Y)));
}
ll_define_primitive_end

ll_define_primitive(immediate, equalQ, _2(x, y), _1(no_side_effect,"#t"))
{
  ll_return(ll_make_boolean(ll_EQ(X, Y)));
}
ll_define_primitive_end

ll_define_primitive(number, equalQ, _2(x, y), _1(no_side_effect,"#t"))
{
  __ll_call_tailv(ll_o(eqvQ), 2);
}
ll_define_primitive_end

/****************************************************************************/

ll_define_primitive(object, equalQ, _2(x, y), _1(no_side_effect,"#t"))
{
  /* Check if they are eq? */
  if ( ll_EQ(X, Y) )
    ll_return(ll_t);

  /* First check if the types are the same */
  if ( ll_NE(ll_TYPE(X), ll_TYPE(Y)) ) {
    /* Check if the object's immutable type counterparts are the same */
    ll_v t1 = ll_call(ll_o(immutable_type), _1(ll_ARG_0));
    ll_v t2 = ll_call(ll_o(immutable_type), _1(ll_ARG_1));
    if ( ll_NE(t1, t2) ) {
      ll_return(ll_f);
    }
  }
  __ll_call_tailv(ll_o(_equalQ), 2);
}
ll_define_primitive_end


/* Binary mixins have slots that cannot be compared using equal? */
ll_define_primitive(not_equal_mixin, _equalQ, _2(x, y), _1(no_side_effect, "#t"))
{
  ll_return(ll_f);
}
ll_define_primitive_end


/* Do not go inside an object during equal?, see equal-mixin. */
ll_define_primitive(object, _equalQ, _2(x, y), _1(no_side_effect, "#t"))
{
  ll_return(ll_f);
}
ll_define_primitive_end


ll_define_primitive(equal_mixin, _equalQ, _2(x, y), _1(no_side_effect, "#t"))
{
  int xi, yi;
  ll_v *x = ll_SLOTS(X), *y = ll_SLOTS(Y);

  xi = ll_unbox_fixnum(ll_call(ll_o(type_size), _1(ll_TYPE(X)))) / sizeof(ll_v);
  yi = ll_unbox_fixnum(ll_call(ll_o(type_size), _1(ll_TYPE(Y)))) / sizeof(ll_v);

  /*
    ll_format(ll_undef, "  %equalQ ~S ~S ~S ~S\n", 4, ll_TYPE(X), ll_BOX_fixnum(xi), ll_TYPE(Y), ll_BOX_fixnum(yi));
  */

  ll_assert(general, xi == yi);
  ll_assert(general, xi > 0);

  /* skip slot[0] (isa) because object::equal? already did it. */
  -- xi;
  ++ x; ++ y;

  /* Work until last slot. */
  if ( -- xi > 0 ) {
    while ( xi -- ) {
      if ( ! ll_equalQ(*(x ++), *(y ++)) )
	ll_return(ll_f);
    }
  }

  /* Tail call on last slot. */
  X = *x;
  Y = *y;
  __ll_call_tailv(ll_o(equalQ), 2);
}
ll_define_primitive_end

/****************************************************************************/

int ll_equalQ(ll_v x, ll_v y)
{
  return ll_unbox_boolean(ll_call(ll_o(equalQ), _2(x, y)));
}

int ll_eqvQ(ll_v x, ll_v y)
{
  return ll_unbox_boolean(ll_call(ll_o(eqvQ), _2(x, y)));
}

#ifdef ll_eqQ
#undef ll_eqQ
#endif

int ll_eqQ(ll_v x, ll_v y)
{
  return ll_EQ(x, y);
}

/****************************************************************************/


#undef X
#undef Y
