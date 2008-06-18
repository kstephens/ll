#ifndef _ll_runtime_h
#define _ll_runtime_h

#ifndef __rcs_id__
#ifndef __rcs_id_ll_runtime_h__
#define __rcs_id_ll_runtime_h__
static const char __rcs_id_ll_runtime_h[] = "$Id: runtime.h,v 1.4 2008/01/06 18:36:33 stephens Exp $";
#endif
#endif /* __rcs_id__ */

#include "ll/value.h" /* ll_v */
#include "ll/global_index.h" /* ll_gi */


extern ll_v _ll_runtime_current; /* FIXME: THREADS */

#if ll_GLOBAL_VARS

#define ll_runtime_current() _ll_runtime_current

#else

extern ll_tsa__runtime _ll_runtime_;
#define ll_runtime_current() ll_BOX_ref(&_ll_runtime_)

#endif

#ifndef ll_runtime
#define ll_runtime(N) (ll_THIS_ISA(_runtime, ll_runtime_current())->_##N)
#endif

#endif

