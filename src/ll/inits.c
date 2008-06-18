#ifndef __rcs_id__
#ifndef __rcs_id_ll_inits_c__
#define __rcs_id_ll_inits_c__
static const char __rcs_id_ll_inits_c[] = "$Id: inits.c,v 1.3 2007/12/23 22:17:35 stephens Exp $";
#endif
#endif /* __rcs_id__ */

#include "ll/init.h"

#ifndef ll_INIT_def
#define ll_INIT_def(N,P,D) extern _ll_init _ll_i_##N;
#include "ll/inits.h"
#endif

_ll_init *_ll_inits[] = {
#ifndef ll_INIT_def
#define ll_INIT_def(N,P,D) &_ll_i_##N,
#include "ll/inits.h"
#endif
  0
};

