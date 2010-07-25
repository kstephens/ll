#ifndef _ll_type_h
#define _ll_type_h

#ifndef __rcs_id__
#ifndef __rcs_id_ll_type_h__
#define __rcs_id_ll_type_h__
static const char __rcs_id_ll_type_h[] = "$Id: type.h,v 1.33 2008/05/26 06:35:03 stephens Exp $";
#endif
#endif /* __rcs_id__ */


/*********************************************************************/
/* Define C structures for LL types. */

/*

ll_ts_T is the C structure for the slots defined in LL type <T>.
ll_tsa_T is the C structure for the object allocated by (make <T>).

ll_THIS_ISA(T, V) uses ll_tsa_T as a top-wired reference.
ll_THIS in a ll_primitive(T, ...) definition uses ll_ts_T and
T's offset into the receiver's object layout as determined by 
method lookup.

*/

/* Forward type declarations. */
#include "ll/prim_type.h"
#include "lcache.h"

#define __ll_t_paste2(N,T)N##T
#define _ll_t_paste2(N,T)__ll_t_paste2(N,T)

#define _ll_t_typedef(N) struct N; typedef struct N N;
#define _ll_type_def(N,T) \
_ll_t_typedef(_ll_t_paste2(ll_ts_,N)) \
_ll_t_typedef(_ll_t_paste2(ll_tsa_,N))
#include "ll/types_begin.h"
#include "ll/type_defs.h"
#include "ll/types_end.h"
#undef _ll_t_typedef

/* ll_ts_X, ll_tsa_X */
#define __ll_runtime_type_h_SKIP
#include "ll/type_struct.h"
#undef  __ll_runtime_type_h_SKIP

/* ll_tsa__runtime */

/* Types needed for runtime */
typedef struct ll_tsa_stack_buffer ll_stack_buffer;
typedef struct ll_tsa_message ll_message;
typedef volatile ll_message  *ll_ar_p;
typedef volatile ll_v                  *ll_val_p;
typedef volatile ll_stack_buffer       *ll_stack_buffer_p; 

#define __ll_type_defs_h "ll/runtime_type.h"
#include "ll/type_struct.h"

#undef __ll_t_paste2
#undef _ll_t_paste2

ll_v ll_type_typechecker(ll_v type);

#endif
