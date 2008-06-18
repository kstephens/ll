#ifndef _ll_op_h
#define _ll_op_h

#ifndef __rcs_id__
#ifndef __rcs_id_ll_op_h__
#define __rcs_id_ll_op_h__
static const char __rcs_id_ll_op_h[] = "$Id: op.h,v 1.5 2008/01/03 12:43:08 stephens Exp $";
#endif
#endif /* __rcs_id__ */

#include "ll/value.h" /* ll_v */
#include "ll/global_index.h" /* ll_gi */

typedef struct _ll_op_init {
  const char *name;
  _ll_gi gi;
  ll_v op;
  ll_v type;
  struct _ll_op_init *locater;
  struct _ll_op_init *setter;
} _ll_op_init;

extern _ll_op_init _ll_op_inits[];

#define ll_SETTER(X) ll_THIS_ISA(settable_operation, (X))->_setter
#define ll_LOCATER(X) ll_THIS_ISA(locatable_operation, (X))->_locater

#endif
