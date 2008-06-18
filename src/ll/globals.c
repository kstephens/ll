#include "ll.h"

/***********************************************************************/
/* global variables */

/* Instantiate global variables. */
#if ll_GLOBAL_VARS
#define ll_g_EXTERN
/* Global variables. */
#include "ll/global_vars.h"
#endif

/* Instantiate global binding structs. */
#if ll_MULTIPLICITY == 0

#ifndef ll_g_def
#if ll_GLOBAL_VARS
#define ll_g_def(N) \
  ll_tsa_binding _ll_gb_##N = { ._locative = ll_BOX_locative(&_ll_gv_##N) };
#else
#define ll_g_def(N) \
  ll_tsa_binding _ll_gb_##N = { ._locative = ll_BOX_locative(&_ll_gb_##N._value) };
#endif
#include "ll/globals.h"
#endif

#endif

/* Instantiate global binding inits. */
#ifndef ll_g_def
#if ll_GLOBAL_VARS
#define ll_g_def(N) \
  _ll_binding_init _ll_gbi_##N = { #N, ll_gi(N), 0, &_ll_gv_##N };
#else
#define ll_g_def(N) \
  _ll_binding_init _ll_gbi_##N = { #N, ll_gi(N), &_ll_gb_##N, 0 };
#endif
_ll_binding_init _ll_gbi_ = { "", 0, 0, 0 };
#include "ll/globals.h"
#endif

_ll_binding_init*
_ll_gbi[] = {
  &_ll_gbi_, /* undefined */
#ifndef ll_g_def
#define ll_g_def(N) &_ll_gbi_##N,
#include "ll/globals.h"
#endif
  0
};


