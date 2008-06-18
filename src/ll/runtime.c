#ifndef __rcs_id__
#ifndef __rcs_id_ll_runtime_c__
#define __rcs_id_ll_runtime_c__
static const char __rcs_id_ll_runtime_c[] = "$Id: runtime.c,v 1.5 2008/05/25 22:36:42 stephens Exp $";
#endif
#endif /* __rcs_id__ */

#include "ll.h"
#include "ll/global_index.h" /* ll_gi_N */


#if ll_MULTIPLICITY
ll_v _ll_runtime_current;
#else
ll_tsa__runtime _ll_runtime_ = {
  ._env_bindings_n = ll_gi_N
};

ll_v _ll_runtime_current = ll_BOX_ref(&_ll_runtime_);
#endif


ll_INIT(runtime, 1, "runtime")
{
#if ll_MULTIPLICITY
  if ( ! ll_INITIALIZED(_ll_runtime_current) ) {
    _ll_runtime_current = ll_BOX_REF(ll_malloc(sizeof(ll_tsa__runtime)));
  }
#endif

  return 0;
}


ll_INIT(runtime2, 101, "runtime type")
{
#if ll_MULTIPLICITY
  _ll_runtime_current = ll_runtime;
#endif

  ll_assert_ref(ll_type(_runtime));
  ll_TYPE_ref(ll_runtime_current()) = ll_type(_runtime);

  ll_assert_ref(ll_runtime_current());
  ll_set_g(_runtime, ll_runtime_current());

#if ll_MULTIPLICITY == 0
  _ll_set_object_nameE(ll_runtime_current(), "#<%runtime>");
#endif

  return 0;
}
