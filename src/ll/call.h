#ifndef _ll_call_h
#define _ll_call_h

/* $Id: call.h,v 1.12 2008/01/06 14:47:27 stephens Exp $ */
/* Copyright (c) 1999-2008 Kurt A. Stephens, ION, Inc. */

#include "ll.h"
#include "stack.h"

/***********************************************/
/* The method lookup subprimitives */

void _ll_lookup(), _ll_lookup_super(ll_v super);

/*************************************************************************/
/* user-level messaging macros */


#if ll_USE_INLINE_CALL_MACROS

/***********************************************/

#include "call_int.h"


/* Push args in reverse order, stack builds down. */
#define _ll_args(A)_ll_ARGS##A

/* Call-site Lookup cache support. */
#if ll_USE_LCACHE

#include "lcache.h"
#define _ll_LOOKUP_CACHE_STATIC static ll_lcache __ll_static_lcache = { { }, __FILE__, __LINE__ };
#define _ll_LOOKUP_CACHE &__ll_static_lcache

#else

#define _ll_LOOKUP_CACHE_STATIC 
#define _ll_LOOKUP_CACHE

#define _ll_lookup_w_cache(CACHE) _ll_lookup()

#endif

#define ll_POP() ({ ll_v __t = _ll_POP(); _ll_VS_POP(_ll_val_sp); __t; })
#define ll_call(OP,ARGS) ({ _ll_args(ARGS); _ll_callv((OP), __ll_ARGC(ARGS)); ll_POP(); })

#define ll_call_super(OP,SUPER,ARGS) ({ _ll_args(ARGS); _ll_call_superv(OP,SUPER,__ll_ARGC(ARGS)); ll_POP(); })

/* Tail-calls with inline args */
#define ll_call_tail(OP,ARGS) do{ _ll_ARGS(ARGS); _ll_call_tailv((OP), __ll_ARGC(ARGS)); }while(0)

/* Tail-calls to super. */
#define ll_call_super_tail(OP,SUPER,ARGS) do { _ll_args(ARGS); _ll_call_super_tailv((OP), (SUPER), __ll_ARGC(ARGS));} while(0)

/***********************************************/

#else

/***********************************************/
/* Use functions for calling. */

#define __ll_ARGLIST_0() ()
#define __ll_ARGLIST_1(_1) (_1)
#define __ll_ARGLIST_2(_1,_2) (_1,_2) 
#define __ll_ARGLIST_3(_1,_2,_3) (_1,_2,_3)
#define __ll_ARGLIST_4(_1,_2,_3,_4) (_1,_2,_3,_4)
#define __ll_ARGLIST_5(_1,_2,_3,_4,_5) (_1,_2,_3,_4,_5)
#define __ll_ARGLIST_6(_1,_2,_3,_4,_5,_6) (_1,_2,_3,_4,_5,_6)
#define __ll_ARGLIST_7(_1,_2,_3,_4,_5,_6,_7) (_1,_2,_3,_4,_5,_6,_7) 
#define __ll_ARGLIST_8(_1,_2,_3,_4,_5,_6,_7,_8) (_1,_2,_3,_4,_5,_6,_7,_8) 
#define __ll_ARGLIST_9(_1,_2,_3,_4,_5,_6,_7,_8,_9) (_1,_2,_3,_4,_5,_6,_7,_8,_9) 
#define __ll_ARGLIST_10(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10) (_1,_2,_3,_4,_5,_6,_7,_8,_9,_10)
#define __ll_ARGLIST(A)__ll_ARGLIST##A

#define __ll_ARGS_NAMES0 _0()
#define __ll_ARGS_NAMES1 _1(_1)
#define __ll_ARGS_NAMES2 _2(_1,_2)
#define __ll_ARGS_NAMES3 _3(_1,_2,_3)
#define __ll_ARGS_NAMES4 _4(_1,_2,_3,_4)
#define __ll_ARGS_NAMES5 _5(_1,_2,_3,_4,_5)
#define __ll_ARGS_NAMES6 _6(_1,_2,_3,_4,_5,_6)
#define __ll_ARGS_NAMES7 _7(_1,_2,_3,_4,_5,_6,_7)
#define __ll_ARGS_NAMES8 _8(_1,_2,_3,_4,_5,_6,_7,_8)
#define __ll_ARGS_NAMES9 _9(_1,_2,_3,_4,_5,_6,_7,_8,_9)
#define __ll_ARGS_NAMES10 _10(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10)
#define __ll_ARGS_NAMES(N)__ll_ARGS_NAMES##N

#define __ll_ARGS_DECL0(T)
#define __ll_ARGS_DECL1(T)T _1
#define __ll_ARGS_DECL2(T)T _1,T _2
#define __ll_ARGS_DECL3(T)T _1,T _2,T _3
#define __ll_ARGS_DECL4(T)T _1,T _2,T _3,T _4
#define __ll_ARGS_DECL5(T)T _1,T _2,T _3,T _4,T _5
#define __ll_ARGS_DECL6(T)T _1,T _2,T _3,T _4,T _5,T _6
#define __ll_ARGS_DECL7(T)T _1,T _2,T _3,T _4,T _5,T _6,T _7
#define __ll_ARGS_DECL8(T)T _1,T _2,T _3,T _4,T _5,T _6,T _7,T _8
#define __ll_ARGS_DECL9(T)T _1,T _2,T _3,T _4,T _5,T _6,T _7,T _8,T _9
#define __ll_ARGS_DECL10(T)T _1,T _2,T _3,T _4,T _5,T _6,T _7,T _8,T _9,T _10
#define __ll_ARGS_DECL(N,T)__ll_ARGS_DECL##N(T)

#define _ll_args_decl(N) _ll_args_##N(__ll_ARGS_DECL(N,ll_v))
void _ll_args_decl(0), 
  _ll_args_decl(1), 
  _ll_args_decl(2), 
  _ll_args_decl(3), 
  _ll_args_decl(4), 
  _ll_args_decl(5), 
  _ll_args_decl(6), 
  _ll_args_decl(7),
  _ll_args_decl(8),
  _ll_args_decl(9),
  _ll_args_decl(10);
#define _ll_args(ARGS) _ll_args##ARGS

ll_v _ll_callv_f(ll_v op, int argc);
ll_v _ll_call_superv_f(ll_v op, ll_v super, int argc);
int _ll_call_tailv_f(ll_v op, int argc);
int _ll_call_super_tailv_f(ll_v op, ll_v super, int argc);


#define ll_call(OP,ARGS) (_ll_args(ARGS), _ll_callv_f((OP), __ll_ARGC(ARGS)))
#define ll_call_super(OP,SUPER,ARGS) (_ll_args(ARGS), _ll_call_superv_f((OP), (SUPER), __ll_ARGC(ARGS)))
#define ll_call_tail(OP,ARGS) do{_ll_args(ARGS); return _ll_call_tailv_f((OP), __ll_ARGC(ARGS)); }while(0)
#define ll_call_super_tail(OP,SUPER,ARGS) do{_ll_args(ARGS); return _ll_call_super_tailv_f((OP), (SUPER), __ll_ARGC(ARGS)); }while(0)

#endif


/***********************************************/
/* arg list helpers */

#define __ll_ARGC_0() 0
#define __ll_ARGC_1(_1) 1
#define __ll_ARGC_2(_1,_2) 2
#define __ll_ARGC_3(_1,_2,_3) 3
#define __ll_ARGC_4(_1,_2,_3,_4) 4
#define __ll_ARGC_5(_1,_2,_3,_4,_5) 5
#define __ll_ARGC_6(_1,_2,_3,_4,_5,_6) 6
#define __ll_ARGC_7(_1,_2,_3,_4,_5,_6,_7) 7
#define __ll_ARGC_8(_1,_2,_3,_4,_5,_6,_7,_8) 8
#define __ll_ARGC_9(_1,_2,_3,_4,_5,_6,_7,_8,_9) 9
#define __ll_ARGC_10(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10) 10
#define __ll_ARGC(A)__ll_ARGC_(A)
#define __ll_ARGC_(A)__ll_ARGC##A


#endif
