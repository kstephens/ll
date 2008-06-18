#ifndef __rcs_id__
#ifndef __rcs_id_ll_op1_h__
#define __rcs_id_ll_op1_h__
static const char __rcs_id_ll_op1_h[] = "$Id: op1.h,v 1.9 2008/01/06 23:10:25 stephens Exp $";
#endif
#endif /* __rcs_id__ */

#ifdef ll_p_def
#undef ll_p_def
#endif

#define ll_p_def(T,N,A,O)ll_o(N)
#include "ll/prims.h"

#ifndef ll_e_NAME
#define ll_e_NAME(N,S)N##_##S
#endif

#define ll_type_def(T,MT)ll_o(T##Q)
#include "ll/type_defs.h"

