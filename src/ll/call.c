#ifndef __rcs_id__
#ifndef __rcs_id_ll_call_c__
#define __rcs_id_ll_call_c__
static const char __rcs_id_ll_call_c[] = "$Id: call.c,v 1.8 2007/09/17 07:01:45 stephens Exp $";
#endif
#endif /* __rcs_id__ */


/*************************************************************************/


#include "ll.h"
#include "call.h"
#include "call_int.h"
#include "assert.h"


/*************************************************************************/
/* argument -> value stack transfer */

#if ! ll_USE_INLINE_CALL_MACROS

#define __ll_ARGS(A)_ll_ARGS(A)
#define _ll_args_def(N) \
void _ll_args_##N(__ll_ARGS_DECL(N,ll_v)) \
{ \
  _ll_ASSERT_STACKS(); \
  __ll_ARGS(__ll_ARGS_NAMES(N)); \
  _ll_ASSERT_STACKS(); \
}

_ll_args_def(0)
_ll_args_def(1)
_ll_args_def(2)
_ll_args_def(3)
_ll_args_def(4)
_ll_args_def(5)
_ll_args_def(6)
_ll_args_def(7)
_ll_args_def(8)
_ll_args_def(9)
_ll_args_def(10)

#undef _ll_args_def

#endif

/*************************************************************************/


ll_v _ll_callv_f(ll_v op, int argc)
{
  _ll_ASSERT_STACKS();
  
  _ll_callv(op, argc);
  {
    ll_v x = _ll_POP();
    _ll_VS_POP(_ll_val_sp);
    _ll_ASSERT_STACKS();
    return x;
  }
}


ll_v _ll_call_superv_f(ll_v op, ll_v super, int argc)
{
  _ll_ASSERT_STACKS();

  _ll_call_superv(op, super, argc);

  {
    ll_v x = _ll_POP();
    _ll_VS_POP(_ll_val_sp);
    _ll_ASSERT_STACKS();
    return x;
  }
}


int _ll_call_tailv_f(ll_v op, int argc)
{
  _ll_ASSERT_STACKS();
  _ll_call_tailv(op, argc);
}


int _ll_call_super_tailv_f(ll_v op, ll_v super, int argc)
{
  _ll_ASSERT_STACKS();
  _ll_call_super_tailv(op, super, argc);
}


/*************************************************************************/
