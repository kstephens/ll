#ifndef _ll_prim_type_h
#define _ll_prim_type_h

#ifndef __rcs_id__
#ifndef __rcs_id_ll_prim_type_h__
#define __rcs_id_ll_prim_type_h__
static const char __rcs_id_ll_prim_type_h[] = "$Id: prim_type.h,v 1.1 2008/01/06 14:47:27 stephens Exp $";
#endif
#endif /* __rcs_id__ */

/***********************************************************************/
/* ll_func: method subprimitive function */

#define ll_func_DECL(X) int X(void)
typedef ll_func_DECL((*ll_func)); /* if returned !0 { perform a tail-call for the returning procedure } */


#endif
