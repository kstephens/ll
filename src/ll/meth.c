#ifndef __rcs_id__
#ifndef __rcs_id_ll_meth_c__
#define __rcs_id_ll_meth_c__
static const char __rcs_id_ll_meth_c[] = "$Id: meth.c,v 1.12 2007/09/08 23:51:19 stephens Exp $";
#endif
#endif /* __rcs_id__ */

#include "ll.h"


/***************************************************************************/


ll_define_primitive(method, method_formals, _1(meth), _1(no_side_effect,"#t"))
{
  if ( ! ll_unbox_boolean(ll_THIS->_formals) ) {
    ll_v x = ll_call(ll_o(property), _2(ll_SELF, ll_s(formals)));
    if ( ll_unbox_boolean(x) ) {
      ll_THIS->_formals = ll_cdr(x);
    } else {
      ll_THIS->_formals = ll_s(args);
    }
  }
  ll_return(ll_THIS->_formals);
}
ll_define_primitive_end


ll_define_primitive(method, method_minargc, _1(meth), _1(no_side_effect,"#t"))
{
  int i = 0;
  ll_v x;

  ll_call(ll_o(method_formals), _1(ll_SELF));
  x = ll_THIS->_formals;

  while ( ll_pairQ(x) ) {
    i ++;
    x = ll_cdr(x);
  }

  ll_return(ll_make_fixnum(i));
}
ll_define_primitive_end


ll_define_primitive(method, locative_properties, _1(meth), _1(no_side_effect,"#t"))
{
  ll_return(ll_make_locative(&ll_THIS->_properties));
}
ll_define_primitive_end


ll_define_primitive(method, method_properties, _1(meth), _1(no_side_effect,"#t"))
{
  ll_return(ll_THIS->_properties);
}
ll_define_primitive_end


ll_define_primitive(method, method_code, _1(meth), _1(no_side_effect,"#t"))
{
  ll_return(ll_THIS->_code);
}
ll_define_primitive_end


ll_define_primitive(method, method_consts, _1(meth), _1(no_side_effect,"#t"))
{
  ll_return(ll_THIS->_consts);
}
ll_define_primitive_end


ll_define_primitive(method, method_env, _1(meth), _1(no_side_effect,"#t"))
{
  ll_return(ll_THIS->_env);
}
ll_define_primitive_end


/***************************************************************************/
