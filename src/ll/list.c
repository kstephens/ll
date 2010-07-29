#ifndef __rcs_id__
#ifndef __rcs_id_ll_list_c__
#define __rcs_id_ll_list_c__
static const char __rcs_id_ll_list_c[] = "$Id: list.c,v 1.19 2008/01/06 18:36:33 stephens Exp $";
#endif
#endif /* __rcs_id__ */

#include "ll.h"
#include "call_int.h"

#include <stdarg.h>
#include <assert.h>


/**********************************************************************/


ll_define_primitive(pair, listQ, _1(object), _1(no_side_effect,"#t"))
{
  ll_call_tail(ll_o(listQ), _1(ll_THIS->_cdr));
}
ll_define_primitive_end


/**********************************************************************/


ll_define_primitive(mutable_pair, __appendE, _2(list, obj), _0())
{
  if ( ll_nullQ(ll_THIS->super_pair._cdr) ) {
    ll_THIS->super_pair._cdr = ll_ARG_1;
    ll_write_barrier_SELF();
  } else {
    ll_SELF = ll_THIS->super_pair._cdr;
    __ll_call_tailv(ll_o(__appendE), 2);
  }
}
ll_define_primitive_end


ll_define_primitive(list, _appendE, _2(list, obj), _0())
{
  if ( ll_nullQ(ll_SELF) ) {
    ll_return(ll_ARG_1);
  } else {
    ll_call(ll_o(__appendE), _2(ll_SELF, ll_ARG_1));
  }
  ll_return(ll_SELF);
}
ll_define_primitive_end


/**********************************************************************/


ll_define_primitive(object, append, __0(lists), _0())
{
  ll_v x = ll_nil, *xp = &x;
  int i;

  if ( ll_ARGC ) {
    for ( i = 0; i < ll_ARGC - 1; i ++ ) {
      ll_LIST_LOOP(ll_ARGV[i], e);
      {
	xp = &ll_CDR(*xp = ll_cons(e, ll_nil));
      }
      ll_LIST_LOOP_END;
    }
    *xp = ll_ARGV[i];
  }

  ll_return(x);
}
ll_define_primitive_end


ll_v _ll_list_reversev(size_t l, const ll_v *v)
{
  ll_v x = ll_nil;

  while ( l -- ) {
    x = ll_cons(*(v ++), x);
  }

  return x;
}


ll_define_primitive(list, reverse, _1(list), _0())
{
  ll_v to = ll_nil;

  ll_LIST_LOOP(ll_SELF, x);
  {
    to = ll_cons(x, to);
  }
  ll_LIST_LOOP_END;

  ll_return(to);
}
ll_define_primitive_end


ll_define_primitive(list, reverseE, _1(list), _0())
{
  ll_v from = ll_SELF;
  ll_v to = ll_nil;

  while ( ! ll_nullQ(from) ) {
    ll_v next = ll_cdr(from);
    
    ll_set_cdrE(from, to);
    to = from;

    from = next;
  }

  ll_return(to);
}
ll_define_primitive_end


ll_define_primitive(pair, list_ref, _2(list,index), _0())
{
  do {
    long i;
    long l = 0;

    i = ll_unbox_fixnum(ll_ARG_1);
    ll_LIST_LOOP(ll_SELF, x);
    {
      if ( i -- == 0 ) {
	ll_return(x);
      }
      l ++;
    }
    ll_LIST_LOOP_END;
    
    ll_ARG_1 = _ll_range_error(ll_s(index), ll_ARG_1, 0, l - 1); 
  } while(0);
}
ll_define_primitive_end


/**********************************************************************/


ll_define_primitive(null, length, _1(list), _1(no_side_effect,"#t"))
{
  ll_return(ll_make_fixnum(0));
}
ll_define_primitive_end


ll_define_primitive(pair, length, _1(list), _1(no_side_effect,"#t"))
{
  size_t l = 1;
  ll_v x = ll_THIS->_cdr;
  while ( ll_NE(x, ll_nil) ) {
    x = ll_call(ll_o(cdr), _1(x));
    l ++;
  }
  ll_return(ll_make_fixnum(l));
}
ll_define_primitive_end


size_t _ll_list_length(ll_v x)
{
  size_t l = 0;
  while ( ll_NE(x, ll_nil) ) {
    x = ll_cdr(x);
    l ++;
  }
  return l;
}

/**********************************************************************/


ll_define_primitive(pair, llCmem, _3(list, obj, test), _1(no_side_effect,"#t"))
{
  ll_v x;

  x = ll_THIS->_car;
  if ( ll_unbox_boolean(ll_call(ll_ARG_2, _2(ll_THIS->_car, ll_ARG_1))) )
    ll_return(ll_SELF);
  ll_SELF = ll_THIS->_cdr;
  __ll_call_tailv(ll_o(llCmem), 3);
}
ll_define_primitive_end


ll_define_primitive(null, llCmem, _3(list, obj, test), _1(no_side_effect,"#t"))
{
  ll_return(ll_f);
}
ll_define_primitive_end


/**********************************************************************/


ll_define_primitive(object, memq, _2(obj, list), _1(no_side_effect,"#t"))
{
  ll_call_tail(ll_o(llCmem), _3(ll_ARG_1, ll_ARG_0, ll_o(eqQ))); 
}
ll_define_primitive_end


ll_v ll_memq(ll_v obj, ll_v list)
{
  return(ll_call(ll_o(memq), _2(obj, list)));
}


/**********************************************************************/


ll_define_primitive(object, memv, _2(obj, list), _1(no_side_effect,"#t"))
{
  ll_call_tail(ll_o(llCmem), _3(ll_ARG_1, ll_ARG_0, ll_o(eqvQ))); 
}
ll_define_primitive_end


ll_v ll_memv(ll_v obj, ll_v list)
{
  return(ll_call(ll_o(memv), _2(obj, list)));
}


/**********************************************************************/


ll_define_primitive(object, member, _2(obj, list), _1(no_side_effect,"#t"))
{
  ll_call_tail(ll_o(llCmem), _3(ll_ARG_1, ll_ARG_0, ll_o(equalQ))); 
}
ll_define_primitive_end


ll_v ll_member(ll_v obj, ll_v list)
{
  return(ll_call(ll_o(member), _2(obj, list)));
}


/**********************************************************************/


ll_define_primitive(pair, llCass, _3(list, obj, test), _1(no_side_effect,"#t"))
{
  ll_v x;

  x = ll_call(ll_o(car), _1(ll_THIS->_car));
  if ( ll_unbox_boolean(ll_call(ll_ARG_2, _2(x, ll_ARG_1))) )
    ll_return(ll_THIS->_car);
  ll_SELF = ll_THIS->_cdr;
  __ll_call_tailv(ll_o(llCass), 3);
}
ll_define_primitive_end


ll_define_primitive(null, llCass, _3(list, obj, test), _1(no_side_effect,"#t"))
{
  ll_return(ll_f);
}
ll_define_primitive_end


/**********************************************************************/


ll_define_primitive(object, assq, _2(obj, list), _1(no_side_effect,"#t"))
{
  ll_call_tail(ll_o(llCass), _3(ll_ARG_1, ll_ARG_0, ll_o(eqQ))); 
}
ll_define_primitive_end


ll_v ll_assq(ll_v obj, ll_v list)
{
  /* fprintf(stderr, "ll_assq sp %p %p %p\n", _ll_val_sp_base, _ll_val_sp, _ll_val_sp_bottom); */
  return(ll_call(ll_o(assq), _2(obj, list)));
}


/**********************************************************************/


ll_define_primitive(object, assv, _2(obj, list), _1(no_side_effect,"#t"))
{
  ll_call_tail(ll_o(llCass), _3(ll_ARG_1, ll_ARG_0, ll_o(eqvQ))); 
}
ll_define_primitive_end


ll_v ll_assv(ll_v obj, ll_v list)
{
  return(ll_call(ll_o(assv), _2(obj, list)));
}


/**********************************************************************/


ll_define_primitive(object, assoc, _2(obj, list), _1(no_side_effect,"#t"))
{
  ll_call_tail(ll_o(llCass), _3(ll_ARG_1, ll_ARG_0, ll_o(equalQ))); 
}
ll_define_primitive_end


ll_v ll_assoc(ll_v obj, ll_v list)
{
  return(ll_call(ll_o(assoc), _2(obj, list)));
}


/**********************************************************************/


ll_define_primitive(list, list__vector, _1(list), _0())
{
  size_t i, l = ll_unbox_fixnum(ll_call(ll_o(length), _1(ll_SELF)));
  ll_v x = ll_SELF;
  ll_v *v = ll_malloc(sizeof(v[0]) * l);
  
  for ( i = 0; i < l; i ++ ) {
    v[i] = ll_call(ll_o(car), _1(x));
    x = ll_call(ll_o(cdr), _1(x));
  }

  ll_return(ll_make_vector(v, l));
}
ll_define_primitive_end


ll_define_primitive(object, list, __0(elements), _0())
{
  ll_return(ll_listv(ll_ARGC, ll_ARGV));
}
ll_define_primitive_end


/*************************************************************/


ll_v ll_listn(int n, ...)
{
  ll_v x = ll_nil, *xp = &x;
  va_list vap;

  va_start(vap, n);

  ll_assert(general, n >= 0);

  while ( n -- ) {
    *xp = ll_cons(va_arg(vap, ll_v), ll_nil);
    ll_write_barrier_ptr(xp);
    xp = &ll_CDR(*xp);
  }

  va_end(vap);

  return x;
}


ll_v ll_listv(int n, const ll_v *v)
{
  ll_v x = ll_nil;

  ll_assert(general, n >= 0);

  v += n;
  while ( n -- ) {
    x = ll_cons(*(-- v), x);
  }

  return x;
}


/*************************************************************/


void _ll_assq_set(ll_v *list, ll_v key, ll_v value)
{
  ll_v *x = list;

  while ( ll_NE(*x, ll_nil) ) {
    ll_v a = ll_car(*x);

    if ( ll_EQ(ll_CAR(a), key) ) {
      /* Replace existing entry */
      ll_set_cdrE(a, value);
      return;
    }
    x = &ll_CDR(*x);
  }

  /* Add a new entry! */
  *list = ll_cons(ll_cons(key, value), *list);
}


ll_v _ll_assq_delete(ll_v *x, ll_v key)
{
  while ( ll_NE(*x, ll_nil) ) {
    ll_v a = ll_car(*x);

    if ( ll_EQ(ll_CAR(a), key) ) {
      /* Remove existing entry */
      *x = ll_cdr(*x);
      ll_write_barrier_ptr(x);
      return a;
    }
    x = &ll_CDR(*x);
  }

  return ll_f;
}


ll_v _ll_assq_to_front(ll_v *list, ll_v key)
{
  ll_v x, *xp;

  x = *(xp = list);
  while ( ll_NE(x,ll_nil) ) {
    ll_v a = ll_car(x);
    if ( ll_EQ(ll_CAR(a), key) ) {
      /* Move to front */
      *xp = ll_cdr(x);
      ll_write_barrier_ptr(xp);

      ll_set_cdrE(x, *list);

      *list = x;
      ll_write_barrier_ptr(list);

      return a;
    }
    x = *(xp = &ll_CDR(x));
  }

  return ll_f;
}


/*************************************************************/
