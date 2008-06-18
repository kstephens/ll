#ifndef __rcs_id__
#ifndef __rcs_id_ll_singleton_c__
#define __rcs_id_ll_singleton_c__
static const char __rcs_id_ll_singleton_c[] = "$Id: singleton.c,v 1.3 2007/09/09 02:54:16 stephens Exp $";
#endif
#endif /* __rcs_id__ */


#include "ll.h"

/*************************************************************************/

ll_define_primitive(singleton_type, make, __1(type, args), _0())
{
  ll_call_super(ll_o(make), ll_f, _1(ll_SELF));
}
ll_define_primitive_end


/*************************************************************************/


ll_define_primitive(singleton_type, instance, 
		    _1(self), 
		    _2(no_side_effect, "#f",
		       doc, "Returns the singleton instance."))
{
  if ( ll_EQ(ll_THIS->_instance, ll_undef) ) {
    ll_THIS->_instance = ll_call_super(ll_o(make), ll_f, _1(ll_SELF));
  }
  ll_return(ll_THIS->_instance);
}
ll_define_primitive_end


/*************************************************************************/


