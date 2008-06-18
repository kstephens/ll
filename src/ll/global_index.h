#ifndef _ll_global_index_h
#define _ll_global_index_h

#ifndef __rcs_id__
#ifndef __rcs_id_ll_global_index_h__
#define __rcs_id_ll_global_index_h__
static const char __rcs_id_ll_global_index_h[] = "$Id: global_index.h,v 1.1 2008/01/03 12:43:08 stephens Exp $";
#endif
#endif /* __rcs_id__ */

/* Global index. */
typedef enum _ll_gi {
  ll_g_UNDEFINED = 0,
#ifndef ll_g_def
#define ll_g_def(X) _ll_gi_##X,
#include "ll/globals.h"
#endif
  ll_gi_N
  } _ll_gi;

/* Returns the global variable index for a name. */
#define _ll_gi(X)_ll_gi_##X
#define ll_gi(X)_ll_gi(X)

#endif
