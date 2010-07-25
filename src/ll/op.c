#ifndef __rcs_id__
#ifndef __rcs_id_ll_op_c__
#define __rcs_id_ll_op_c__
static const char __rcs_id_ll_op_c[] = "$Id: op.c,v 1.26 2008/05/26 12:21:38 stephens Exp $";
#endif
#endif /* __rcs_id__ */

#include "ll.h"
#include "op.h"
#include "assert.h"

/************************************************************************/

ll_define_primitive(operation, initialize, __1(op, lambdaQ), _0())
{
  ll_THIS->_lambdaQ = ll_ARGC == 2 ? ll_ARG_1 : ll_undef;
  /* ll_THIS->_lambda_type = ll_undef; */
#if ll_USE_TYPE_METH_ALIST
  ll_THIS->_type_meth_alist = ll_nil;
#endif
  ll_THIS->_immutableQ = ll_f;
  ll_THIS->_properties = ll_nil;
#if ll_USE_OPERATION_VERSION
  ll_THIS->_version = ll_BOX_fixnum(0);
#endif
  ll_write_barrier_SELF();
  ll_return(ll_SELF);
}
ll_define_primitive_end


#if ll_USE_TYPE_METH_ALIST
ll_define_primitive(operation, type_meth_alist, _1(self), _1(no_side_effect, "#t"))
{
  /* Provide unified result even if operation is a lambda. */
  if ( ll_NE(ll_THIS->_lambda_type, ll_undef) ) {
    ll_return(ll_cons(
		      ll_cons(ll_THIS->_lambda_type, ll_THIS->_lambdaQ),
		      ll_nil));
  } else {
    ll_return(ll_THIS->_type_meth_alist);
  }
}
ll_define_primitive_end
#endif


ll_define_primitive(operation, locative_properties, _1(self), _1(no_side_effect,"#t"))
     /* Access properties. */
{
  ll_return(ll_make_locative(&ll_THIS->_properties));
}
ll_define_primitive_end


static __inline void _ll_init_operation(ll_tsa_operation *x)
{
  x->_lambdaQ = 
  x->_lambda_type =
  x->_cache_type =
  x->_cache_impl =
  x->_cache_offset =
  x->_cache_method = ll_undef;
#if ll_USE_TYPE_METH_ALIST
  x->_type_meth_alist = ll_nil;
#endif
  x->_immutableQ = ll_f;
  x->_properties = ll_nil;
#if ll_USE_OPERATION_VERSION
  x->_version = ll_BOX_fixnum(0);
#endif
  ll_write_barrier_pure(x);
}

 
ll_v _ll_make_operation()
{
  ll_v o = _ll_allocate_type(ll_type(operation));
  ll_tsa_operation *x = ll_THIS_ISA(operation, o);
  _ll_init_operation(x);
  return o;
}


/************************************************************************/


ll_define_primitive(settable_operation, initialize, _1(op), _0())
{
  ll_THIS->_setter = ll_call(ll_o(make), _1(ll_type(operation)));
  ll_write_barrier_SELF();
  ll_call_super_tail(ll_o(initialize), ll_f, _1(ll_SELF));
}
ll_define_primitive_end


ll_define_primitive(settable_operation, getter, _1(op), _1(no_side_effect, "#t"))
{
  ll_return(ll_SELF);
}
ll_define_primitive_end


ll_define_primitive(settable_operation, locative_setter, _1(op), _1(no_side_effect, "#t"))
{
  ll_return(ll_make_locative(&ll_THIS->_setter));
}
ll_define_primitive_end


ll_define_primitive(settable_operation, setter, _1(op), _1(no_side_effect, "#t"))
{
  ll_return(ll_THIS->_setter);
}
ll_define_primitive_end


ll_define_primitive(settable_operation, set_setterE, _2(op, setter), _0())
{
  ll_THIS->_setter = ll_ARG_1;
  ll_write_barrier_SELF();
}
ll_define_primitive_end


ll_v _ll_make_settable_operation()
{
  ll_v o = _ll_allocate_type(ll_type(settable_operation));
  ll_tsa_settable_operation *x = ll_THIS_ISA(settable_operation, o);
  _ll_init_operation((void*) x);
  ll_SETTER(o) = _ll_make_operation();
  ll_write_barrier_pure(x);
  return o;
}


/************************************************************************/


ll_define_primitive(locatable_operation, initialize, _1(op), _0())
{
  ll_THIS->_locater = ll_call(ll_o(make), _1(ll_type(operation)));
  ll_write_barrier_SELF();
  ll_call_super_tail(ll_o(initialize), ll_f, _1(ll_SELF));
}
ll_define_primitive_end


ll_define_primitive(locatable_operation, locater, _1(op), _1(no_side_effect, "#t"))
{
  ll_return(ll_THIS->_locater);
}
ll_define_primitive_end


ll_define_primitive(locatable_operation, locative_locater, _1(op), _1(no_side_effect, "#t"))
{
  ll_return(ll_make_locative(&ll_THIS->_locater));
}
ll_define_primitive_end


ll_define_primitive(locatable_operation, set_locaterE, _2(op, locater), _0())
{
  ll_THIS->_locater = ll_ARG_1;
}
ll_define_primitive_end


ll_v _ll_make_locatable_operation()
{
  ll_v o = _ll_allocate_type(ll_type(locatable_operation));
  ll_tsa_locatable_operation *x = ll_THIS_ISA(locatable_operation, o);
  _ll_init_operation((void*) x);
  ll_SETTER(o) = _ll_make_operation();
  ll_write_barrier_pure(x);
  ll_LOCATER(o) = _ll_make_operation();
  ll_write_barrier_pure(x);
  return o;
}


