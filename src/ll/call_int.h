#ifndef _ll_call_int_h
#define _ll_call_int_h

/* $Id: call_int.h,v 1.14 2008/05/26 06:35:03 stephens Exp $ */
/* Copyright (c) 1999, Kurt A. Stephens, ION, Inc. */

#include "ll/stack.h"

/***********************************************/
/* ll_call() and ll_call_super() take a parenthesized arg list 
 * that are pushed on to the stack in reverse order because the stack grows downward.
 */

#define _ll_ARGS_0() _ll_VS_PROBE(1) /* Leave room for return value! */
#define _ll_ARGS_1(_1) _ll_VS_PROBE(1);_ll_PUSH(_1)
#define _ll_ARGS_2(_1,_2) _ll_VS_PROBE(2);_ll_PUSH(_2);_ll_PUSH(_1)
#define _ll_ARGS_3(_1,_2,_3) _ll_VS_PROBE(3);_ll_PUSH(_3);_ll_PUSH(_2);_ll_PUSH(_1)
#define _ll_ARGS_4(_1,_2,_3,_4) _ll_VS_PROBE(4);_ll_PUSH(_4);_ll_PUSH(_3);_ll_PUSH(_2);_ll_PUSH(_1)
#define _ll_ARGS_5(_1,_2,_3,_4,_5) _ll_VS_PROBE(5);_ll_PUSH(_5);_ll_PUSH(_4);_ll_PUSH(_3);_ll_PUSH(_2);_ll_PUSH(_1)
#define _ll_ARGS_6(_1,_2,_3,_4,_5,_6) _ll_VS_PROBE(6);_ll_PUSH(_6);_ll_PUSH(_5);_ll_PUSH(_4);_ll_PUSH(_3);_ll_PUSH(_2);_ll_PUSH(_1)
#define _ll_ARGS_7(_1,_2,_3,_4,_5,_6,_7) _ll_VS_PROBE(7);_ll_PUSH(_7);_ll_PUSH(_6);_ll_PUSH(_5);_ll_PUSH(_4);_ll_PUSH(_3);_ll_PUSH(_2);_ll_PUSH(_1)
#define _ll_ARGS_8(_1,_2,_3,_4,_5,_6,_7,_8) _ll_VS_PROBE(8);_ll_PUSH(_8);_ll_PUSH(_7);_ll_PUSH(_6);_ll_PUSH(_5);_ll_PUSH(_4);_ll_PUSH(_3);_ll_PUSH(_2);_ll_PUSH(_1)
#define _ll_ARGS_9(_1,_2,_3,_4,_5,_6,_7,_8,_9) _ll_VS_PROBE(9);_ll_PUSH(_9);_ll_PUSH(_8);_ll_PUSH(_7);_ll_PUSH(_6);_ll_PUSH(_5);_ll_PUSH(_4);_ll_PUSH(_3);_ll_PUSH(_2);_ll_PUSH(_1)
#define _ll_ARGS_10(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10) _ll_VS_PROBE(10);_ll_PUSH(_10);_ll_PUSH(_9);_ll_PUSH(_8);_ll_PUSH(_7);_ll_PUSH(_6);_ll_PUSH(_5);_ll_PUSH(_4);_ll_PUSH(_3);_ll_PUSH(_2);_ll_PUSH(_1)
#define _ll_ARGS(A)_ll_ARGS##A

/*
** Tail-recursion is implemented in C as follows:
**
** CALL:
**   1. Push the arguments on the value stack in reverse order.  
**      Stack pointer is the new argument vector.
**   2. Push a new message with the procedure and the number of arguments.
**   3. Keep calling the top message's function if it returns ! 0.
**   4. Pop the message.
**   5. Return value is on the top of the value stack.
**
** RETURN(X):
**   1. Reset the stack pointer to the end of the current argument vector.
**   2. Push the value X on the stack.
**   3. Return 0 to force the caller's loop (CALL #3) to terminate.
**
** TAIL-CALL:
**   1. Push the arguments on the value stack in reverse order.
**     Stack: | NARG1 | NARG2 | .... | ARGV0 | ARGV1 | ....
**              ^ SP                   ^ ARGV
**   2. Reset the stack pointer to the end of the current argument vector.
**   3. Copy the new arguments by pushing them on the value stack in reverse order.
**   4. Reuse the current message to store the new message.
**   5. Return 1 to force the caller to process the new message (this allows the C stack to unwind)
**
**
** Note: Do not pass a reference to a C stack allocated object to a tail call.
** Example:  DO NOT DO THE FOLLOWING:

ll_define_primitive(object, foo, _0(), _0())
{
  ll_tsa_pair cons_on_stack;
  ll_v c = ll_make_ref(&cons_on_stack);

  ...

  ll_call_tail(ll_o(bar), _1(c));
}
ll_define_primitive_end
*/

/***********************************************/

#if ll_USE_FILE_LINE
#define __ll_call_FILE_LINE \
    ll_AR_FILE = __FILE__; \
    ll_AR_LINE = __LINE__
#else
#define __ll_call_FILE_LINE
#endif

#define __ll_call_method_func(METH) \
  (ll_THIS_ISA(method, (METH))->_func)()

#define __ll_call_tail_method_func(METH) \
  return(__ll_call_method_func(METH))

#define __ll_lookup_begin(OP, ARGC, LOOKUP) \
do { \
  _ll_LOOKUP_CACHE_STATIC; \
  { \
    register ll_v __ll_op = (OP); \
    register int __ll_argc = (ARGC); \
    _ll_AR_PROBE(1); \
    (-- _ll_ar_sp)->super_object._isa = ll_type(message); \
    ll_AR_OP = __ll_op; \
    ll_AR_ARGC = __ll_argc; \
    __ll_call_FILE_LINE; \
  } \
  ll_AR_ARGV = (ll_v*) _ll_val_sp; \
  ll_AR_ARGV_SB = (void *) _ll_val_stack_buffer; \
  ll_AR_TYPE = _ll_ar_sp[1]._type; \
  /* Remember current implementer type for _ll_lookup_super(#f) */ \
  ll_AR_DB_AT_RTN = ll_f; \
  LOOKUP

#define __ll_lookup_end() \
  ++ _ll_ar_sp; \
}while(0)


/* stack contains: rcvr arg ... */
#define __ll_callv(OP, ARGC, LOOKUP) \
  __ll_lookup_begin(OP, ARGC, LOOKUP); \
  while ( __ll_call_method_func(ll_AR_METH) ) \
    /* TILL NOT TAIL CALL */; \
  __ll_lookup_end()

/* stack contains: rcvr arg ... */
#define _ll_callv(OP, ARGC) \
  __ll_callv(OP, ARGC, _ll_lookup_w_cache(_ll_LOOKUP_CACHE))

/* stack contains: self arg ... */
#define _ll_call_superv(OP, SUPER, ARGC) do { \
  __ll_callv(OP, ARGC, _ll_lookup_super(SUPER)); \
} while (0)


/* Tail call with args already in place (forwarding call) */
#define ___ll_call_tailv(OP, ARGC, LOOKUP) \
do { \
  _ll_LOOKUP_CACHE_STATIC; \
  ll_AR_OP = (OP); \
  ll_AR_ARGC = (ARGC); \
  ll_AR_ARGV = (ll_v *) _ll_val_sp; \
  ll_AR_ARGV_SB = (void *) _ll_val_stack_buffer; \
  __ll_call_FILE_LINE; \
  LOOKUP; \
  return 1; \
}while(0)


#define __ll_call_tailv(OP, ARGC) \
  ___ll_call_tailv(OP, ARGC, _ll_lookup_w_cache(_ll_LOOKUP_CACHE))


/*
** Tail-call with args to be moved into place:
** 1. Remember end of the argument list.
** 2. Unwind the value stack pointer to the previous frame.
**   (pop current argument vector)
** 3. Probe the stack for room for new arguments.
** 4. Repush the argument list into previous frame.
*/
#define __ll_call_tail_MOVE_ARGS(OP, ARGC) \
  ll_v __op = (OP); \
  int __argc = (ARGC); \
  { \
    int ___argc = __argc; \
    const ll_v *__p = ((ll_v*) _ll_val_sp) + __argc; \
    if ( _ll_val_stack_buffer != ll_AR_ARGV_SB ) \
      _ll_val_stack_buffer_switch(ll_AR_ARGV_SB); \
    _ll_VS_POP(ll_ARGV + ll_ARGC); \
    _ll_VS_PROBE(___argc); \
    while ( ___argc -- ) \
      __ll_PUSH(*(-- __p)); \
  }


#define _ll_call_tailv(OP, ARGC) \
do { \
  __ll_call_tail_MOVE_ARGS(OP, ARGC); \
  __ll_call_tailv(__op, __argc); \
} while(0)


/* 
** Super tail-call with args to be moved into place.
*/
#define _ll_call_super_tailv(OP, SUPER, ARGC) \
do { \
  __ll_call_tail_MOVE_ARGS(OP, ARGC); \
  ___ll_call_tailv(__op, __argc, _ll_lookup_super(SUPER)); \
} while(0)


/*************************************************************************/
/* Lookup cache support. */

#define _ll_callv_w_cache(OP, ARGC, CACHE) \
  __ll_callv(OP, ARGC, _ll_lookup_w_cache(CACHE))

#define _ll_call_tailv_w_cache(OP, ARGC, CACHE) \
do { \
  __ll_call_tail_MOVE_ARGS(OP, ARGC); \
  ___ll_call_tailv(__op, __argc, _ll_lookup_w_cache(CACHE));    \
} while(0)

/* TODO call_super, call_super_tail */

/*************************************************************************/

#endif
