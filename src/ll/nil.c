#ifndef __rcs_id__
#ifndef __rcs_id_ll_nil_c__
#define __rcs_id_ll_nil_c__
static const char __rcs_id_ll_nil_c[] = "$Id: nil.c,v 1.16 2008/01/04 01:20:45 stephens Exp $";
#endif
#endif /* __rcs_id__ */

#include "ll.h"


/**********************************************************************/


ll_define_primitive(null, initialize, __1(object,inits), _1(no_side_effect,"#t"))
{
  /* nil is created "by hand" below. */
  ll_return(ll_nil);
}
ll_define_primitive_end


ll_define_primitive(null, clone, _1(object), _1(no_side_effect,"#t"))
{
  ll_return(ll_SELF);
}
ll_define_primitive_end


ll_define_primitive(null, make_immutable, _1(object), _1(no_side_effect,"#t"))
{
  ll_return(ll_SELF);
}
ll_define_primitive_end


/**********************************************************************/


ll_INIT(nil, 80, "nil, ()")
{
  ll_v nil;

#if ll_MULTIPLICTY
  nil = _ll_allocate_type(ll_type(null));
#else
  nil = ll_nil;
  ll_TYPE_ref(nil) = ll_type(null);
#endif
  ll_set_g(nil, nil);

  _ll_set_object_nameE(ll_nil, "()");
  ll_set_g(_fluid_bindings, ll_nil);

  return 0;
}


ll_INIT(nil2, 295, "nil readonly")
{
  ll_v binding;
  binding = ll_call(ll_o(_binding_or_error), 
		    _3(ll_call(ll_o(SenvironmentS), _0()), 
		       ll_s(nil),
		       ll_t));

  ll_call(ll_o(make_readonlyE), _1(binding));
  
  return 0;
}

/**********************************************************************/
