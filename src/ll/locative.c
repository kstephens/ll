#ifndef __rcs_id__
#ifndef __rcs_id_ll_locative_c__
#define __rcs_id_ll_locative_c__
static const char __rcs_id_ll_locative_c[] = "$Id: locative.c,v 1.10 2008/05/28 11:00:16 stephens Exp $";
#endif
#endif /* __rcs_id__ */

#include "ll.h"


/************************************************************************/


ll_v * ll_unbox_locative(ll_v x)
{
  while ( ! ll_ISA_locative(x) ) {
    x = _ll_typecheck_error(ll_type(locative), x);
  }
  return ll_UNBOX_locative(x);
}


/************************************************************************/


ll_define_primitive(locative, contents, _1(locative), _1(no_side_effect,"#t"))
{
  ll_return(*ll_UNBOX_locative(ll_SELF));
}
ll_define_primitive_end


ll_define_primitive(locative, locative_contents, _1(locative), _1(no_side_effect,"#t"))
{
  ll_return(ll_SELF);
}
ll_define_primitive_end


ll_define_primitive(locative, set_contentsE, _2(locative, value), _0())
{
  ll_v *loc = ll_UNBOX_locative(ll_SELF);
  *loc = ll_ARG_1;
  ll_write_barrier(loc);
}
ll_define_primitive_end


