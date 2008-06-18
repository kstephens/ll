#ifndef __rcs_id__
#ifndef __rcs_id_ll_undef_c__
#define __rcs_id_ll_undef_c__
static const char __rcs_id_ll_undef_c[] = "$Id: undef.c,v 1.7 2007/12/23 20:09:45 stephens Exp $";
#endif
#endif /* __rcs_id__ */


#include "ll.h"

ll_INIT(undef, 70, "undefined and unspecified value")
{
  ll_set_g(__undef, _ll_allocate_type(ll_type(undefined)));
  ll_assert_ref(ll_undef);

  ll_set_g(__unspec, _ll_allocate_type(ll_type(unspecified)));
  ll_assert_ref(ll_unspec);

  _ll_set_object_nameE(ll_undef, "#u");
  _ll_set_object_nameE(ll_unspec, "#U");

  return 0;
}
