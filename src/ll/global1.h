#ifndef __rcs_id__
#ifndef __rcs_id_ll_global1_h__
#define __rcs_id_ll_global1_h__
static const char __rcs_id_ll_global1_h[] = "$Id: global1.h,v 1.11 2008/01/06 23:10:25 stephens Exp $";
#endif
#endif /* __rcs_id__ */

#ifdef ll_set_g
#undef ll_set_g
#endif
#define ll_set_g(X,Y)ll_g(X),(Y)

#include "ll.h"

#undef ll_p_def
#define ll_p_def(T,N,A,O)ll_g(LT##T##GT)ll_g(N)
#include "ll/prims.h"

#undef ll_macro_def
#define ll_macro_def(T,N,A)ll_g(LT##T##GT)ll_g(N)ll_g(ll_mn(N))
#include "ll/macros.h"

#undef ll_o_def
#define ll_o_def(N)ll_g(N)
#include "ll/ops.h"

#undef ll_type_def
#define ll_type_def(T,N)ll_g(LT##T##GT)
#include "ll/type_defs.h"

#undef ll_DEBUG_def
#define ll_DEBUG_def(X)_ll_g(_ll_DEBUG_SYM(X))
#include "ll/debugs.h"


