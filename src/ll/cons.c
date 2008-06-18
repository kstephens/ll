#ifndef __rcs_id__
#ifndef __rcs_id_ll_cons_c__
#define __rcs_id_ll_cons_c__
static const char __rcs_id_ll_cons_c[] = "$Id: cons.c,v 1.20 2008/05/24 20:56:34 stephens Exp $";
#endif
#endif /* __rcs_id__ */

#include "ll.h"


/*************************************************************************/
/* type queries */


ll_define_primitive(pair, immutable_type, _1(pair), _1(no_side_effect, "#t"))
{
  ll_return(ll_type(pair));
}
ll_define_primitive_end


ll_define_primitive(pair, mutable_type, _1(pair), _1(no_side_effect, "#t"))
{
  ll_return(ll_type(mutable_pair));
}
ll_define_primitive_end


/*************************************************************************/
/* Initialization */


ll_define_primitive(pair, initialize, _3(self, car, cdr), _0())
{
  ll_THIS->_car = ll_ARG_1;
  ll_THIS->_cdr = ll_ARG_2;
  ll_write_barrier_SELF();
  ll_return(ll_SELF);
}
ll_define_primitive_end


ll_define_primitive(object, cons, _2(car, cdr), _0())
{
  ll_call_tail(ll_o(make), _3(ll_type(mutable_pair), ll_SELF, ll_ARG_1));
}
ll_define_primitive_end


/*************************************************************************/


ll_define_primitive(pair, make_immutable, _1(pair), _0())
{
  ll_TYPE_ref(ll_SELF) = ll_type(pair);
  ll_write_barrier_SELF();
  ll_return(ll_SELF);
}
ll_define_primitive_end


ll_define_primitive(pair, make_mutable, _1(pair), _0())
{
  ll_SELF = ll_cons(ll_THIS->_car, ll_THIS->_cdr);
  ll_return(ll_SELF);
}
ll_define_primitive_end


/*************************************************************************/

ll_define_primitive(pair, car, _1(pair), _1(no_side_effect, "#t"))
{
  ll_return(ll_THIS->_car);
}
ll_define_primitive_end


ll_define_primitive(pair, cdr, _1(pair), _1(no_side_effect, "#t"))
{
  ll_return(ll_THIS->_cdr);
}
ll_define_primitive_end


/*************************************************************************/


ll_define_primitive(mutable_pair, locative_car, _1(pair), _1(no_side_effect, "#t"))
{
  ll_return(ll_make_locative(&ll_THIS->super_pair._car));
}
ll_define_primitive_end


ll_define_primitive(mutable_pair, locative_cdr, _1(pair), _1(no_side_effect, "#t"))
{
  ll_return(ll_make_locative(&ll_THIS->super_pair._cdr));
}
ll_define_primitive_end


ll_define_primitive(mutable_pair, set_carE, _2(pair, new_car), _0())
{
  ll_THIS->super_pair._car = ll_ARG_1;
  ll_write_barrier_SELF();
}
ll_define_primitive_end


ll_define_primitive(mutable_pair, set_cdrE, _2(pair, new_cdr), _0())
{
  ll_THIS->super_pair._cdr = ll_ARG_1;
  ll_write_barrier_SELF();
}
ll_define_primitive_end


/*************************************************************************/

__inline
ll_v _ll_cons(ll_v type, ll_v a, ll_v d)
{
  ll_v x = _ll_allocate_type(type);
  ll_SLOTS(x)[1] = a;
  ll_SLOTS(x)[2] = d;
  ll_write_barrier_pure(x);
  return x;
}


ll_v _ll_car(ll_v x)
{
  return ll_SLOTS(x)[1];
}


ll_v _ll_cdr(ll_v x)
{
  return ll_SLOTS(x)[2];
}


/*************************************************************/


ll_v ll_cons(ll_v a, ll_v d)
{
  return _ll_cons(ll_type(mutable_pair), a, d);
}


ll_v ll_append(ll_v x, ll_v y)
{
  return ll_call(ll_o(append), _2(x, y));
}


ll_v ll_immutable_cons(ll_v a, ll_v d)
{
  return _ll_cons(ll_type(pair), a, d);
}


int ll_pairQ(ll_v x)
{
  ll_v x_type = ll_TYPE(x);
  if ( ll_EQ(x_type, ll_type(mutable_pair)) || 
       ll_EQ(x_type, ll_type(pair)) ) {
    return 1;
  } else {
    return ll_unbox_boolean(ll_call(ll_o(pairQ), _1(x)));
  }
}


ll_v ll_car(ll_v x)
{
  ll_v x_type = ll_TYPE(x);
  if ( ll_EQ(x_type, ll_type(mutable_pair)) || 
       ll_EQ(x_type, ll_type(pair)) ) {
    return ll_CAR(x);
  } else {
    return ll_call(ll_o(car), _1(x));
  }
}


ll_v ll_cdr(ll_v x)
{
  ll_v x_type = ll_TYPE(x);
  if ( ll_EQ(x_type, ll_type(mutable_pair)) ||
       ll_EQ(x_type, ll_type(pair)) ) {
    return ll_CDR(x);
  } else {
    return ll_call(ll_o(cdr), _1(x));
  }
}


void ll_set_carE(ll_v x, ll_v v)
{
  ll_v x_type = ll_TYPE(x);
  if ( ll_EQ(x_type, ll_type(mutable_pair)) ) {
    ll_CAR(x) = v;
    ll_write_barrier_pure(x);
  } else {
    ll_call(ll_o(set_carE), _2(x, v));
  }
}


void ll_set_cdrE(ll_v x, ll_v v)
{
  ll_v x_type = ll_TYPE(x);
  if ( ll_EQ(x_type, ll_type(mutable_pair)) ) {
    ll_CDR(x) = v;
    ll_write_barrier_pure(x);
  } else {
    ll_call(ll_o(set_cdrE), _2(x, v));
  }
}


ll_v ll_locative_car(ll_v x)
{
  ll_v x_type = ll_TYPE(x);
  if ( ll_EQ(x_type, ll_type(mutable_pair)) ) {
    return(ll_make_locative(&ll_CAR(x)));
  } else {
    return(ll_call(ll_o(locative_car), _1(x)));
  }
}


ll_v ll_locative_cdr(ll_v x)
{
  ll_v x_type = ll_TYPE(x);
  if ( ll_EQ(x_type, ll_type(mutable_pair)) ) {
    return(ll_make_locative(&ll_CDR(x)));
  } else {
    return(ll_call(ll_o(locative_cdr), _1(x)));
  }
}


/*************************************************************************/
/* conviences */


ll_define_primitive(object, safe_car, _1(x), _0())
     /* Returns the car or #f.*/
{
  ll_return(ll_f);
}
ll_define_primitive_end


ll_define_primitive(object, safe_cdr, _1(x), _0())
     /* Returns the cdr or #f.*/
{
  ll_return(ll_f);
}
ll_define_primitive_end


ll_define_primitive(pair, safe_car, _1(x), _0())
     /* Returns the car or #f.*/
{
  ll_return(ll_THIS->_car);
}
ll_define_primitive_end


ll_define_primitive(pair, safe_cdr, _1(x), _0())
     /* Returns the cdr or #f.*/
{
  ll_return(ll_THIS->_cdr);
}
ll_define_primitive_end


/*************************************************************************/
