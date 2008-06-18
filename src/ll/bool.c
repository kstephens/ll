#ifndef __rcs_id__
#ifndef __rcs_id_ll_bool_c__
#define __rcs_id_ll_bool_c__
static const char __rcs_id_ll_bool_c[] = "$Id: bool.c,v 1.13 2008/01/04 03:33:45 stephens Exp $";
#endif
#endif /* __rcs_id__ */

#include "ll.h"


ll_define_primitive(object, not, _1(x), _1(no_side_effect,"#t"))
{
  ll_return(ll_make_boolean(! ll_unbox_boolean(ll_SELF)));
}
ll_define_primitive_end


ll_INIT(bool, 100, "#t, #f")
{
#if ll_GLOBAL_VARS
  ll_set_g(__t, _ll_allocate_type(ll_type(boolean)));
  ll_set_g(__f, _ll_allocate_type(ll_type(boolean)));
#else
  ll_TYPE_ref(ll_t) = ll_type(boolean);
  ll_TYPE_ref(ll_f) = ll_type(boolean);
#endif

  ll_assert_ref(ll_t);
  ll_assert_ref(ll_f);

  _ll_set_object_nameE(ll_t, "#t");
  _ll_set_object_nameE(ll_f, "#f");

  return 0;
}
