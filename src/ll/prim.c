#ifndef __rcs_id__
#ifndef __rcs_id_ll_prim_c__
#define __rcs_id_ll_prim_c__
static const char __rcs_id_ll_prim_c[] = "$Id: prim.c,v 1.18 2008/01/06 18:36:33 stephens Exp $";
#endif
#endif /* __rcs_id__ */


#include "ll.h"
#include <string.h> /* strcat() */


/*************************************************************************/


void _ll_initialize_primitive(ll_v x, ll_func prim, const char *name, ll_v formals, ll_v props)
{
  ll_tsa_method *m = ll_THIS_ISA(method,x);

  m->_func = prim;
  m->_formals = formals;
  m->_code = _ll_make_symbol_(name);
  m->_properties = props;
}


ll_v _ll_make_primitive(ll_func prim, const char *name, ll_v formals, ll_v props)
{
  ll_v x = _ll_allocate_type(ll_type(primitive));
  _ll_initialize_primitive(x, prim, name, formals, props);
  return x;
}


/***************************************************************************/


ll_define_primitive(object, _identity,
		    __1(self,args), 
		    _2(no_side_effect,"#t",
		       doc,"Always returns receiver."))
{
  ll_return(ll_SELF);
}
ll_define_primitive_end


ll_define_primitive(object, _true,
		    __0(args), 
		    _2(no_side_effect, "#t",
		       doc, "Always returns true."))
{
  ll_return(ll_t);
}
ll_define_primitive_end


ll_define_primitive(object, _false,
		    __0(args), 
		    _2(no_side_effect, "#t",
		       doc, "Always returns false."))
{
  ll_return(ll_f);
}
ll_define_primitive_end


