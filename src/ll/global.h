#ifndef _ll_global_h
#define _ll_global_h

#ifndef __rcs_id__
#ifndef __rcs_id_ll_global_h__
#define __rcs_id_ll_global_h__
static const char __rcs_id_ll_global_h[] = "$Id: global.h,v 1.8 2008/01/06 18:36:33 stephens Exp $";
#endif
#endif /* __rcs_id__ */

/* Enable undefined global value checks */
#ifndef ll_g_CHECK
#define ll_g_CHECK 0
#endif

#include "ll/global_index.h" /* ll_gi */

/* Global variable binding initializations. */
typedef struct _ll_binding_init {
  const char *_name;
  _ll_gi _index;
  ll_tsa_binding *_bp;
  ll_v *_gvp;
} _ll_binding_init;

extern _ll_binding_init
#ifndef ll_g_def
#define ll_g_def(X) _ll_gbi_##X,
#include "ll/globals.h"
#endif
  _ll_gbi_;

/* Collection of all global binding inits indexed by ll_gi(N). */
extern _ll_binding_init* _ll_gbi[];

/* Global string name by global index. */
#define _ll_gn(I) _ll_gbi[I]->_name
#define ll_gn(I) _ll_gn(ll_gi(I))

/* Enable use of global variables for all globally bound ll vars. */
#if ll_GLOBAL_VARS

#include "ll/global_vars.h"
#define ll_g_(X)_ll_gv_##X

#else /* ! ll_GLOBAL_VARS */

/* Use global binding structs for global values. */

extern ll_tsa_binding
#ifndef ll_g_def
#define ll_g_def(N) _ll_gb_##N,
#include "ll/globals.h"
#endif
_ll_gb_
;

#define _ll_gb(N)ll_BOX_ref(&_ll_gb_##N)
#define ll_gb(N)_ll_gb(N)

#if 0
/* Use bindings locatives */
#define ll_g_(N) (*ll_UNBOX_locative(ll_THIS_ISA(binding, ll_gb(N))->_locative))
#else
/* Go straight to binding value, assume locative = &value */
#define ll_g_(N) (ll_THIS_ISA(binding, ll_gb(N))->_value)
#endif

#endif /* ! ll_GLOBAL_VARS */

#ifndef ll_g
#define ll_g(N)ll_g_(N)
#endif

#ifndef ll_set_g
#define ll_set_g(N,V) (ll_g(N) = (V))
#endif

#ifndef ll_g
/* Don't bother checking for undefined global values */
#define ll_g(N) _ll_get_gi(_ll_g(N))
#define ll_set_g(N,V) (_ll_get_gi(_ll_g(N)) = (V))
#endif

#if ll_g_CHECK
/* Checking for undefined global values */
ll_v _ll_global_get(ll_v binding);
ll_v _ll_global_set(ll_v binding, ll_v value);
#undef ll_g
#define ll_g(N)_ll_global_get(ll_gb(N))
#undef ll_set_g
#define ll_set_g(N,Y)_ll_global_set(ll_gb(N),(Y))
#endif

#ifndef _ll_get_gi
#define _ll_get_gi(I) (*ll_UNBOX_locative(_ll_gbi[I]->_bp->_locative))
#define _ll_set_gi(I,V) (_ll_get_gi(I) = (V))
#endif

#endif
