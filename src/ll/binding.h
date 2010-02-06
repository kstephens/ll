#ifndef _ll_binding_h

#ifndef __rcs_id__
#ifndef __rcs_id_ll_binding_h__
#define __rcs_id_ll_binding_h__
static const char __rcs_id_ll_binding_h[] = "$Id: ll.h,v 1.52 2008/05/25 23:08:41 stephens Exp $";
#endif
#endif /* __rcs_id__ */

#define _ll_binding_h


static inline
ll_v __ll_binding_value(ll_v binding)
{
  ll_tsa_binding *ll_THIS = ll_THIS_ISA(binding, binding);
  if ( ! ll_unbox_boolean(ll_THIS->_locative) ) {
    return(_ll_undefined_variable_error(ll_THIS->_symbol));
  }
  return *ll_UNBOX_locative(ll_THIS->_locative);
}

ll_v _ll_binding_value(ll_v binding);
void _ll_set_binding_valueE(ll_v binding, ll_v value);
ll_v _ll_locative_binding_value(ll_v binding);

#endif
