#ifndef __rcs_id__
#ifndef __rcs_id_ll_symbol1_h__
#define __rcs_id_ll_symbol1_h__
static const char __rcs_id_ll_symbol1_h[] = "$Id: symbol1.h,v 1.10 2008/01/06 23:10:25 stephens Exp $";
#endif
#endif /* __rcs_id__ */

#include "ll.h"

#define _ll_type_def_slot(T,N,S)ll_s(S)
#include "ll/type_defs.h"

#ifdef ll_g
#undef ll_g
#endif
#define ll_g(N)ll_s(N)
#include "ll/global1.h"

#define ll_bc_def(N,NARGS,SM,NSA)ll_s(N)
#include "ll/bcs.h"

