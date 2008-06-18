#ifndef __rcs_id__
#ifndef __rcs_id_ll_trace_c__
#define __rcs_id_ll_trace_c__
static const char __rcs_id_ll_trace_c[] = "$Id: trace.c,v 1.12 2008/01/06 18:36:33 stephens Exp $";
#endif
#endif /* __rcs_id__ */

#include "ll.h"
#include "call_int.h"

/*
** This should never be called directly.
*/
ll_define_primitive(object, __trace_method_apply, __0(args), _0())
{
  ll_AR_METH = ll_THIS_ISA(trace_method, ll_AR_METH)->_meth;
  _ll_print_trace(ll_undef);
  __ll_call_tail_method_func(ll_AR_METH);
}
ll_define_primitive_end


ll_define_primitive(trace_method,initialize, _3(self,type,op), _0())
{
   ll_v isa = ll_SLOTS(ll_SELF)[0];

  /* Copy <object>::%trace_method-apply */
  *(ll_tsa_primitive*)ll_THIS = *ll_THIS_ISA(primitive, ll_p(object, __trace_method_apply));

  ll_SLOTS(ll_SELF)[0] = isa; /* Fix isa */

  /* Remember who to forward to */
  ll_THIS->_type = ll_ARG_1;
  ll_THIS->_op = ll_ARG_2;
  ll_write_barrier_SELF();
  ll_THIS->_meth = ll_call(ll_o(lookup), _2(ll_THIS->_type, ll_THIS->_op));
  ll_write_barrier_SELF();

  ll_call(ll_o(_add_method), _3(ll_THIS->_type, ll_THIS->_op, ll_SELF));

  ll_return(ll_SELF);
}
ll_define_primitive_end

/****************************************************************************/

ll_define_primitive(type, trace, _2(type, op), _0())
{
  ll_call_tail(ll_o(make), _3(ll_type(trace_method), ll_SELF, ll_ARG_1));
}
ll_define_primitive_end

/****************************************************************************/

ll_define_primitive(method, _untrace, _1(self), _1(no_side_effect,"#t"))
{
  /* DO NOTHING */
}
ll_define_primitive_end

ll_define_primitive(trace_method, _untrace, _1(self), _0())
{
  ll_call_tail(ll_o(_add_method), _3(ll_THIS->_type, ll_THIS->_op, ll_THIS->_meth));
}
ll_define_primitive_end

ll_define_primitive(type, untrace, _2(type, op), _0())
{
  ll_v meth = ll_call(ll_o(lookup), _2(ll_SELF, ll_ARG_1));
  ll_call_tail(ll_o(_untrace), _1(meth));
}
ll_define_primitive_end

/****************************************************************************/
