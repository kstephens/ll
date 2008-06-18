#ifndef __rcs_id__
#ifndef __rcs_id_ll_macro_c__
#define __rcs_id_ll_macro_c__
static const char __rcs_id_ll_macro_c[] = "$Id: macros.c,v 1.2 2008/01/03 13:29:51 stephens Exp $";
#endif
#endif /* __rcs_id__ */


#include "ll.h"


/*********************************************************/


ll_INIT(macro, 260, "system")
{
  /* Install the macro definitions */
  struct {
    ll_v  macro_op;
    ll_v  macro_name;
  } x[] = {
#ifndef ll_macro_def
#define ll_macro_def(TYPE,NAME,ARGS) { ll_o(ll_mn(NAME)), ll_s(NAME) },
#include "ll/macros.h"
#endif
    { 0 }
  };
  int i;
  ll_v env = ll_call(ll_o(SenvironmentS), _0());

  for ( i = 0; x[i].macro_op; i ++ ) {
    ll_assert_ref(x[i].macro_op);
    ll_assert_ref(x[i].macro_name);
    ll_call(ll_o(_define_macro), _3(env, x[i].macro_name, x[i].macro_op));
  }

  return 0;
}


/*********************************************************/


