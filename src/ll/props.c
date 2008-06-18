#ifndef __rcs_id__
#ifndef __rcs_id_ll_props_c__
#define __rcs_id_ll_props_c__
static const char __rcs_id_ll_props_c[] = "$Id: props.c,v 1.7 2007/12/19 04:04:20 stephens Exp $";
#endif
#endif /* __rcs_id__ */

#include "ll.h"

ll_define_primitive(properties_mixin, initialize, __1(self, args), _0())
{
  ll_call(ll_o(set_propertiesE), _2(ll_SELF, ll_nil));
}
ll_define_primitive_end

ll_define_primitive(properties_mixin, properties, _1(self), _1(no_side_effect,"#t"))
{
  ll_v ploc = ll_call(ll_o(locative_properties), _1(ll_SELF));
  ll_return(*ll_unbox_locative(ploc));
}
ll_define_primitive_end

ll_define_primitive(properties_mixin, set_propertiesE, _2(self, props), _1(no_side_effect,"#t"))
{
  ll_v ploc = ll_call(ll_o(locative_properties), _1(ll_SELF));
  *ll_unbox_locative(ploc) = ll_ARG_1;
  ll_write_barrier(ll_UNBOX_locative(ploc));
}
ll_define_primitive_end

ll_define_primitive(properties_mixin, property, _2(self, prop), _1(no_side_effect,"#t"))
{
  ll_v ploc = ll_call(ll_o(locative_properties), _1(ll_SELF));
  ll_return(_ll_assq_to_front(ll_unbox_locative(ploc), ll_ARG_1));
}
ll_define_primitive_end

ll_define_primitive(properties_mixin, set_propertyE, _3(self, prop, value), _0())
{
  ll_v ploc = ll_call(ll_o(locative_properties), _1(ll_SELF));
  _ll_assq_set(ll_unbox_locative(ploc), ll_ARG_1, ll_ARG_2);
  ll_return(ll_SELF);
}
ll_define_primitive_end


ll_define_primitive(properties_mixin, incr_propertyE, __2(self, prop, incr), _0())
{
  ll_v value = ll_call(ll_o(property), _2(ll_SELF, ll_ARG_1));
  ll_v incr = ll_ARGC >= 2 ? ll_ARG_2 : ll_BOX_fixnum(1);

  if ( ! ll_unbox_boolean(value) ) {
    value = ll_BOX_fixnum(0);
  }
  value = ll__ADD(value, incr);
  ll_call_tail(ll_o(set_propertyE), _3(ll_SELF, ll_ARG_1, value)); 
}
ll_define_primitive_end


ll_define_primitive(properties_mixin, unset_propertyE, _2(self, prop), _0())
{
  ll_v ploc = ll_call(ll_o(locative_properties), _1(ll_SELF));
  ll_return(_ll_assq_delete(ll_unbox_locative(ploc), ll_ARG_1));
}
ll_define_primitive_end

