#ifndef _ll_stack_h
#define _ll_stack_h

#ifndef __rcs_id__
#ifndef __rcs_id_ll_stack_h__
#define __rcs_id_ll_stack_h__
static const char __rcs_id_ll_stack_h[] = "$Id: stack.h,v 1.11 2008/05/26 06:35:03 stephens Exp $";
#endif
#endif /* __rcs_id__ */

#include "ll/assert.h"


/****************************************************************************/
/* stack buffer */



/****************************************************************************/
/* message stack */


/****************************************************************************/
/* message protocol */

/*
Use two stacks to make proper tail recursion more efficient.

1. Message stack:
  Contains:
  1.1. the operation sent: op
  1.2. the argument list vector with the receiver at 0: argv, argc
  1.3. the method found: meth
  1.4. the type the method was found in: type
  1.5. the offset from address of the receiver to the ivar block for the type the method was found in: type_offset

2. Values stack: for both arguments and return values.

Both stacks grow from high addresses to low addresses.
Eventually, will implement stack buffers for call/cc.
*/


/****************************************************************************/
/* The message stack */

#if 0
extern ll_ar_p _ll_ar_sp, _ll_ar_sp_bottom, _ll_ar_sp_base;
extern ll_stack_buffer_p _ll_ar_stack_buffer;
#else
#define _ll_ar_sp           ll_runtime(ar_sp)
#define _ll_ar_sp_bottom    ll_runtime(ar_sp_bottom)
#define _ll_ar_sp_base      ll_runtime(ar_sp_base)
#define _ll_ar_stack_buffer ll_runtime(ar_stack_buffer)
#endif

#define ll_AR ll_make_ref(_ll_ar_sp)
#define ll_AR_ARGC (_ll_ar_sp->_argc)
#define ll_AR_ARGV (_ll_ar_sp->_argv)
#define ll_AR_ARGV_SB (_ll_ar_sp->_argv_sb)
#define ll_AR_OP (_ll_ar_sp->_op)
#define ll_AR_TYPE (_ll_ar_sp->_type)
#define ll_AR_TYPE_OFFSET (_ll_ar_sp->_type_offset)
#define ll_AR_METH (_ll_ar_sp->_meth)
#define ll_AR_DB_AT_RTN (_ll_ar_sp->_db_at_rtn)
#define ll_AR_FILE (_ll_ar_sp->_file)
#define ll_AR_LINE (_ll_ar_sp->_line)

#define ll_ARGC ll_AR_ARGC
#define ll_ARGV ll_AR_ARGV

/*
** NOTE: These macros may be redefined
** during inlining of hot methods as bytecodes.
*/
#define ll_ARG_0 ll_ARGV[0]
#define ll_ARG_1 ll_ARGV[1]
#define ll_ARG_2 ll_ARGV[2]
#define ll_ARG_3 ll_ARGV[3]
#define ll_ARG_4 ll_ARGV[4]
#define ll_ARG_5 ll_ARGV[5]
#define ll_ARG_6 ll_ARGV[6]
#define ll_ARG_7 ll_ARGV[7]
#define ll_ARG_8 ll_ARGV[8]
#define ll_ARG_9 ll_ARGV[9]

#define ll_SELF ll_ARG_0
#define ll_RCVR ll_SELF
#define ll_RCVR_TYPE (ll_AR_ARGC ? ll_TYPE(ll_RCVR) : ll_type(object))

#define _ll_AS_CHECK(N) 0


/* push and pop message stack buffers. */
extern void _ll_ar_push(int n);
extern void _ll_ar_pop();


#define _ll_AR_PROBE(N) if ( _ll_ar_sp - (N) < _ll_ar_sp_base ) _ll_ar_push(N)
#define _ll_AR_POP()    if ( _ll_ar_sp >= _ll_ar_sp_bottom ) _ll_ar_pop()


/****************************************************************************/
/* The value stack */

#if 0
extern ll_val_p _ll_val_sp, _ll_val_sp_beg, _ll_val_sp_end;
extern ll_stack_buffer_p _ll_val_stack_buffer;
#else
#define _ll_val_sp           ll_runtime(val_sp)
#define _ll_val_sp_beg       ll_runtime(val_sp_beg)
#define _ll_val_sp_end       ll_runtime(val_sp_end)
#define _ll_val_stack_buffer ll_runtime(val_stack_buffer)
#endif


#define _ll_PEEK()  (*_ll_val_sp)

#define __ll_POP()   (*(_ll_val_sp++))
#define __ll_PUSH(X) (*(--_ll_val_sp)=(X))

static inline ll_v _ll_POP()
{
  ll_assert(stack, _ll_val_sp < _ll_val_sp_end);
  return __ll_POP();
}
static inline void _ll_PUSH(ll_v x)
{
  ll_assert(stack, _ll_val_sp > _ll_val_sp_beg);
  __ll_PUSH(x);
}

#define _ll_POPN(N)  (*(_ll_val_sp+=(N)))
#define _ll_PUSHN(N) (_ll_val_sp-=(N))

/* push and pop value stack buffers. */
extern void _ll_val_stack_buffer_switch(struct ll_tsa_stack_buffer *sb);
extern void _ll_vs_push(int n);
extern ll_v *_ll_vs_pop(ll_v *new_sp);

/* <= because we might need an extra for implied self in _ll_call_superv() */
#define _ll_VS_PROBE(N) \
  if ( _ll_val_sp - (N) < _ll_val_sp_beg ) _ll_vs_push(N)

/* Check to see if we need to pop stack buffers here. */
#define _ll_VS_POP(NEW_SP) do { \
  ll_v *__ll_new_sp = (ll_v *) (NEW_SP); \
  _ll_val_sp = (ll_v *) ((__ll_new_sp < _ll_val_sp_end) ? __ll_new_sp : _ll_vs_pop(__ll_new_sp)); \
} while ( 0 )


#if 0
#include "ll/assert.h"
#define _ll_ASSERT_VAL_STACK() \
  ll_assert_msg(stack, _ll_val_sp_beg <= _ll_val_sp && _ll_val_sp <= _ll_val_sp_end, ("%p %p %p value stack buffer over/underrun", _ll_val_sp_beg, _ll_val_sp, _ll_val_sp_end))

#define _ll_ASSERT_AR_STACK() \
  ll_assert_msg(stack, _ll_ar_sp_base <= _ll_ar_sp && _ll_ar_sp <= _ll_ar_sp_bottom, ("%p %p %p ar stack buffer over/underrun", _ll_ar_sp_base, _ll_ar_sp, _ll_ar_sp_bottom))

#define _ll_ASSERT_STACKS() _ll_ASSERT_VAL_STACK(); _ll_ASSERT_AR_STACK()
#else
#define _ll_ASSERT_AR_STACK()
#define _ll_ASSERT_VAL_STACK()
#define _ll_ASSERT_STACKS()
#endif

/***********************************************************************/
/* Stack state */

struct ll_tsa_stack_state;

void _ll_get_stack_state(struct ll_tsa_stack_state *ss);
void _ll_set_stack_state(const struct ll_tsa_stack_state *ss);

#endif
