#ifndef _ll_bcode_h

#ifndef __rcs_id__
#ifndef __rcs_id_ll_bcode_h__
#define __rcs_id_ll_bcode_h__
static const char __rcs_id_ll_bcode_h[] = "$Id: bcode.h,v 1.8 2007/09/13 20:44:26 stephens Exp $";
#endif
#endif /* __rcs_id__ */

#define _ll_bcode_h

typedef enum ll_bc {
  _ll_bc_NULL = 0,
#ifndef ll_bc_def
#define ll_bc_def(X,NARGS,SM,NSA) ll_bc_##X,
#include "ll/bcs.h"
#endif
  _ll_bc_LAST
} ll_bc;

#ifndef ll_bc
#define ll_bc(X,NARGS,SM,NSA) ll_bc_##X
#endif


/*********************************************************************/
/* bytecode vector. */

typedef unsigned char ll_bc_t;

int _ll_bc_get_args(const ll_bc_t **_bc, long *args);
int _ll_bc_max_stack_depth(const ll_bc_t *bc);
int _ll_bc_stack_motion(const ll_bc_t **_bc);
int _ll_bc_nsa(const ll_bc_t **_bc);

#endif
