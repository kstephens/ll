#ifndef _ll_type_def


#ifndef __ll_type_defs_h
#define __ll_type_defs_h "ll/type_defs.h"
#endif

/* ll_ts_X definitions. */
#define _ll_type_def(N,T) struct _ll_t_paste2(ll_ts_,N) {
#define _ll_type_def_slot(N,T,S) T _ll_t_paste2(_,S);
#define _ll_type_def_end(N) };
#include "ll/types_begin.h"
#include __ll_type_defs_h
#include "ll/types_end.h"

/* ll_tsa_X definitions. */
#define _ll_type_def(N,T) struct _ll_t_paste2(ll_tsa_,N) {
#define _ll_type_def_super(T,S) _ll_t_paste2(ll_tsa_,S) _ll_t_paste2(super_,S);
#define _ll_type_def_slot(N,T,S) T _ll_t_paste2(_,S);
#define _ll_type_def_end(N) };
#include "ll/types_begin.h"
#include __ll_type_defs_h
#include "ll/types_end.h"

#endif


#undef __ll_type_defs_h
