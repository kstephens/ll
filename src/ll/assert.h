#ifndef _ll_ASSERT_H
#define _ll_ASSERT_H

#ifndef __rcs_id__
#ifndef __rcs_id_ll_assert_h__
#define __rcs_id_ll_assert_h__
static const char __rcs_id_ll_assert_h[] = "$Id: assert.h,v 1.8 2008/01/06 14:47:27 stephens Exp $";
#endif
#endif /* __rcs_id__ */


#include "ll/config.h"

enum ll_assert {
#ifndef ll_ASSERT_ENABLE
  ll_ASSERT_ENABLE = 1,
#endif

  ll_assert_disable  = 0,
  ll_assert_enable   = ll_ASSERT_ENABLE,

  ll_assert_general  = ll_assert_enable,
  ll_assert_init     = ll_assert_enable,
  ll_assert_lookup   = ll_assert_enable,
  ll_assert_mem      = ll_assert_enable,
  ll_assert_stack    = 0, // ll_assert_enable,
  ll_assert_ir       = ll_assert_enable,
  ll_assert_bc       = ll_assert_enable,
  ll_assert_env      = ll_assert_enable,
  ll_assert_sig      = ll_assert_enable,
  ll_assert_type     = ll_assert_enable,
  ll_assert_prim     = ll_assert_enable,
  ll_assert_fixnum   = ll_assert_enable,
  ll_assert_locative = ll_assert_enable,
  ll_assert_flonum   = ll_assert_enable,
  ll_assert_ref      = ll_assert_enable,
  ll_assert_
};

#include <assert.h>


void __ll_assert_1(const char *name, const char *expr, int value, const char *file, int line, const char *func);
void __ll_assert_2(const char *format, ...);

#ifndef __GNUC__
#define __FUNCTION__ 0
#endif

#define _ll_assert(T,ES,E,M,C)do {if ( ll_assert_##T > 0 && !(E)) { __ll_assert_1(#T, ES, (C), __FILE__, __LINE__, __FUNCTION__); __ll_assert_2 M; } }while(0)

#if defined(NDEBUG) || defined(NASSERT)
#define ll_assert_msg(T,E,M) ((void)0)
#define ll_assert(T,E) ((void)0)
#else
#define ll_assert_msg(T,E,M)_ll_assert(T,#E,E,M,1)
#define ll_assert(T,E)_ll_assert(T,#E,E,(0),1)
#endif

#define ll_warn_msg(T,E,M)_ll_assert(T,#E,E,M,0)
#define ll_warn(T,E)_ll_assert(T,#E,E,(0),0)

#if ll_ASSERT_IS_WARN
#undef ll_assert_msg
#define ll_assert_msg ll_warn_msg
#undef ll_assert
#define ll_assert ll_warn
#endif

#define ll_assert_fixnum(E)   ll_assert_msg(fixnum, ll_ISA_fixnum(E), ("%s = %s", #E, ll_po(E)))
#define ll_assert_locative(E) ll_assert_msg(locative, ll_ISA_locative(E), ("%s = %s", #E, ll_po(E)))
#define ll_assert_flonum(E)   ll_assert_msg(flonum, ll_ISA_flonum(E), ("%s = %s", #E, ll_po(E)))
#define ll_assert_ref(E)      ll_assert_msg(ref, ll_ISA_ref(E), ("%s = %s", #E, ll_po(E)))

#endif

