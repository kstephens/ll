/* Use global variables for global values. */
#if ll_GLOBAL_VARS

#ifndef ll_g_EXTERN
#define ll_g_EXTERN extern
#endif

ll_g_EXTERN
ll_v 
#ifndef ll_g_def
#define ll_g_def(N) _ll_gv_##N,
#include "ll/globals.h"
#endif
  ll_gv__;

#undef ll_g_EXTERN

#endif

