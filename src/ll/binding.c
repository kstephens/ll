#ifndef __rcs_id__
#ifndef __rcs_id_ll_binding_c__
#define __rcs_id_ll_binding_c__
static const char __rcs_id_ll_binding_c[] = "$Id: binding.c,v 1.17 2008/01/06 18:36:33 stephens Exp $";
#endif
#endif /* __rcs_id__ */

#include "ll.h"

/***********************************************************************/


ll_define_primitive(binding, initialize, _2(binding, symbol), _0())
{
  ll_THIS->_symbol = ll_ARG_1;
  ll_THIS->_locative = ll_f;
  ll_THIS->_readonlyQ = ll_f;
  ll_THIS->_value = ll_undef;
  ll_THIS->_macro = ll_f;
  ll_THIS->_properties = ll_nil;
  ll_write_barrier_SELF();
  ll_call_super_tail(ll_o(initialize), ll_f, _1(ll_SELF));
}
ll_define_primitive_end


/**************************************************************************/


ll_define_primitive(binding, _write_default_contents, _3(obj, port, op), _0())
{
  ll_format(ll_ARG_1, "~S 'locative ~S 'macro ~S 'properties ~S", 4,
	    ll_THIS->_symbol,
	    ll_THIS->_locative,
	    ll_THIS->_macro,
	    ll_THIS->_properties);
}
ll_define_primitive_end


/**************************************************************************/


ll_define_primitive(binding, binding_symbol, _1(binding), _1(no_side_effect,"#t"))
{
  ll_return(ll_THIS->_symbol);
}
ll_define_primitive_end


/**************************************************************************/


ll_define_primitive(binding, binding_value, _1(binding), _1(no_side_effect,"#t"))
{
  if ( ! ll_unbox_boolean(ll_THIS->_locative) ) {
    ll_return(_ll_undefined_variable_error(ll_THIS->_symbol));
  }

  ll_return(*ll_unbox_locative(ll_THIS->_locative));
}
ll_define_primitive_end


ll_v _ll_binding_value(ll_v binding)
{
  if ( ll_TYPE(binding) == ll_type(binding) ) {
    return __ll_binding_value(binding);
  } else {
    return ll_call(ll_o(binding_value), _1(binding));
  }
}


ll_define_primitive(binding, set_binding_valueE, _2(binding, value), _0())
{
  if ( ll_unbox_boolean(ll_call(ll_o(property), _2(ll_SELF, ll_s(readonly)))) ) {
    /* IMPLEMENT: handle proper error */
    ll_return(_ll_readonly_variable_error(ll_THIS->_symbol));
  }

  if ( ! ll_unbox_boolean(ll_THIS->_locative) ) {
    ll_THIS->_locative = ll_make_locative(&ll_THIS->_value);
    ll_write_barrier_SELF();
  }

  *ll_unbox_locative(ll_THIS->_locative) = ll_ARG_1;
  /* WRITE BARRIER */
}
ll_define_primitive_end


void _ll_set_binding_valueE(ll_v binding, ll_v value)
{
  ll_call(ll_o(set_binding_valueE), _2(binding, value));
}


ll_define_primitive(binding, locative, _1(binding), _1(no_side_effect, "#t"))
{
  ll_return(ll_THIS->_locative);
}
ll_define_primitive_end


ll_define_primitive(binding, set_locativeE, _2(binding, locative), _0())
{
  ll_THIS->_locative = ll_ARG_1;
  ll_write_barrier_SELF();
}
ll_define_primitive_end


ll_define_primitive(binding, locative_binding_value, _1(binding), _1(no_side_effect, "#t"))
{
  if ( ! ll_unbox_boolean(ll_THIS->_locative) ) {
    ll_return(_ll_undefined_variable_error(ll_THIS->_symbol));
  }

  if ( ll_unbox_boolean(ll_call(ll_o(property), _2(ll_SELF, ll_s(readonly)))) ) {
    /* IMPLEMENT: handle proper error. */
    ll_return(_ll_readonly_variable_error(ll_THIS->_symbol));
  }

  ll_assert_locative(ll_THIS->_locative);
  ll_return(ll_THIS->_locative);
}
ll_define_primitive_end


ll_v _ll_locative_binding_value(ll_v binding)
{
  return ll_call(ll_o(locative_binding_value), _1(binding));
}


/**************************************************************************/


ll_define_primitive(binding, binding_macro, _1(binding), _1(no_side_effect, "#t"))
{
  ll_return(ll_THIS->_macro);
}
ll_define_primitive_end


ll_define_primitive(binding, set_binding_macroE, _2(binding, value), _0())
{
  ll_THIS->_macro = ll_ARG_1;
  ll_write_barrier_SELF();
}
ll_define_primitive_end


/**************************************************************************/


ll_define_primitive(binding, readonlyQ, _1(binding), _1(no_side_effect, "#t"))
{
  /* HACK For env.c: _uninit */
  /*
  if ( ll_ISA_locative(ll_THIS->_readonlyQ) ) {
    ll_return(ll_f);
  }
  */
  ll_return(ll_THIS->_readonlyQ);
}
ll_define_primitive_end


ll_define_primitive(binding, make_readonlyE, _1(binding), _0())
{
  ll_assert_ref(ll_t);
  ll_THIS->_readonlyQ = ll_t;
}
ll_define_primitive_end


ll_define_primitive(binding, locative_properties, _1(binding), _1(no_side_effect, "#t"))
{
  ll_return(ll_make_locative(&ll_THIS->_properties));
}
ll_define_primitive_end


/**************************************************************************/
