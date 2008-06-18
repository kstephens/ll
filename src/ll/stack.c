#ifndef __rcs_id__
#ifndef __rcs_id_ll_stack_c__
#define __rcs_id_ll_stack_c__
static const char __rcs_id_ll_stack_c[] = "$Id: stack.c,v 1.20 2008/05/26 07:48:08 stephens Exp $";
#endif
#endif /* __rcs_id__ */

#include "ll.h"
#include "ll/assert.h"

#include <assert.h>


#ifndef ll_STACK_DEBUG
#define ll_STACK_DEBUG 0
#endif


#if 1
/*
 * This does not quite work yet.
 * Stack buffers appear to get overrun during tak.scm
 *  -- kurt 2007/09/17
 */
#define ll_VS_SIZE 256
#else
#define ll_VS_SIZE 1024
#endif

#define ll_AR_SIZE 256

#ifndef ll_VS_SIZE
#define ll_VS_SIZE 32
#endif

#ifndef ll_AR_SIZE
#define ll_AR_SIZE 16
#endif


/**************************************************************************/
/* value stack; used for arguments and return values */


#if 0
volatile ll_v 
*_ll_val_sp,          /* Current location in the stack buffer*/
  *_ll_val_sp_beg,    /* _ll_val_stack_buffer->_beg */
  *_ll_val_sp_end;    /* _ll_val_stack_buffer->_end */
volatile ll_tsa_stack_buffer *_ll_val_stack_buffer;
#endif


/**************************************************************************/
/* message stack */


#if 0
volatile ll_message 
*_ll_ar_sp,         /* Current message. */
  *_ll_ar_sp_bottom,  /* Bottom of stack */ 
  *_ll_ar_sp_base;
volatile struct ll_tsa_stack_buffer *_ll_ar_stack_buffer;
#endif


ll_tsa_stack_buffer *_ll_make_stack_buffer(size_t size, size_t element_size, ll_tsa_stack_buffer *prev)
{
  ll_tsa_stack_buffer *sb = ll_malloc(sizeof(*sb));

  sb->super_object._isa = ll_type(stack_buffer);
  sb->_prev = prev;
  sb->_size = size;
  element_size = (element_size + sizeof(ll_v) - 1) / sizeof(ll_v);
  sb->_element_size = element_size;
  element_size = (element_size * size);
  sb->_beg = (ll_v *) ll_malloc(sizeof(sb->_beg[0]) * element_size);
  sb->_sp =
    sb->_end = sb->_beg + element_size;

  return sb;
}


/**************************************************************************/
/* Manupulate stack buffers. */


void _ll_val_stack_buffer_switch(ll_tsa_stack_buffer *sb)
{
  if ( ll_STACK_DEBUG ) {
    fprintf(stderr, "\nll: _ll_val_stack_buffer_switch: FROM %p: %p %p %p\n",
	    (void*) _ll_val_stack_buffer,
	    (void*) _ll_val_sp_beg, 
	    (void*) _ll_val_sp, 
	    (void*) _ll_val_sp_end);
  }

  /* Save current val stack pointer in prev stack buffer. */
  if ( _ll_val_stack_buffer ) {
    _ll_val_stack_buffer->_sp = (ll_v *) _ll_val_sp;
  }

  /* Set current val stack pointer. */
  _ll_val_stack_buffer = sb;
  _ll_val_sp = sb->_sp;
  _ll_val_sp_beg = sb->_beg;
  _ll_val_sp_end = sb->_end;

  if ( _ll_val_sp ) {
    _ll_ASSERT_STACKS();
  }

  /* Always keep an extra around. */
  /*
    _ll_PUSH(ll_undef);
    _ll_PEEK() = _ll_BOX
    _ll_PUSH(ll_undef);
  */

  if ( ll_STACK_DEBUG ) {
    fprintf(stderr, "ll: _ll_val_stack_buffer_switch:   TO %p: %p %p %p\n", 
	    (void*) sb,
	    (void*) _ll_val_sp_beg, 
	    (void*) _ll_val_sp, 
	    (void*) _ll_val_sp_end);
  }

  _ll_ASSERT_VAL_STACK();
}


void _ll_vs_push(int n)
     /* Create a new stack buffer on the stack. */
{
  ll_tsa_stack_buffer *sb;

  if ( n < ll_VS_SIZE ) {
    n = ll_VS_SIZE;
  }
      
  sb = _ll_make_stack_buffer(n, 
			     sizeof(ll_v), 
			     (ll_tsa_stack_buffer *) _ll_val_stack_buffer);

  _ll_val_stack_buffer_switch(sb);
}



/*
** Imagine a method context with the following:

[ stack-buffer #1 ]  -->  [ stack-buffer #2 ]  -->  [ stack-buffer #3 ]
  ^                            ^             ^
  |                            |             |
  locals                       argv          argv + argc

1. argv is first thing on stack-buffer #2
2. locals are current on stack-buffer #1

ll_return does:

  _ll_val_sp = ll_ARGV + ll_ARGC (the locals and argument pop operation).

_ll_val_sp doesn't reside in the current stack buffer.

*/

ll_v *_ll_vs_pop(ll_v *new_sp)
{
  if ( ll_STACK_DEBUG ) {
    fprintf(stderr, "ll: _ll_vs_pop:          %p: %p %p %p\n",
	    (void*) _ll_val_stack_buffer,
	    (void*) _ll_val_sp_beg, 
	    (void*) new_sp, 
	    (void*) _ll_val_sp_end);
  }
  
  _ll_val_stack_buffer_switch(_ll_val_stack_buffer->_prev);

  if ( ll_STACK_DEBUG ) {
    fprintf(stderr, "ll: _ll_vs_pop: RESTORE: %p: %p %p %p\n", 
	    (void*) _ll_val_stack_buffer,
	    (void*) _ll_val_sp_beg, 
	    (void*) _ll_val_sp, 
	    (void*) _ll_val_sp_end);
  }

  ll_assert(stack, _ll_val_sp_beg);
  ll_assert(stack, _ll_val_sp);
  ll_assert(stack, _ll_val_sp_end);
  
  _ll_ASSERT_STACKS();

  return (ll_v *) _ll_val_sp;
}


void _ll_vs_pop_loop(ll_v *new_sp)
     /* pop the current stack buffer */
{
  if ( ll_STACK_DEBUG ) {
    fprintf(stderr, "\n");
  }
 
  while ( ! (_ll_val_sp_beg <= 
	     new_sp && 
	     new_sp <= 
	     _ll_val_sp_end) ) {
    if ( ll_STACK_DEBUG ) {
      fprintf(stderr, "ll: _ll_vs_pop:          %p: %p %p %p\n",
	      (void*) _ll_val_stack_buffer,
	      (void*) _ll_val_sp_beg, 
	      (void*) new_sp, 
	      (void*) _ll_val_sp_end);
    }

    _ll_val_stack_buffer_switch(_ll_val_stack_buffer->_prev);

    ll_assert(stack, _ll_val_sp_beg);
    ll_assert(stack, _ll_val_sp_end);
    
    if ( ll_STACK_DEBUG ) {
      fprintf(stderr, "ll: _ll_vs_pop: RESTORE: %p: %p %p %p\n", 
	      (void*) _ll_val_stack_buffer,
	      (void*) _ll_val_sp_beg, 
	      (void*) _ll_val_sp, 
	      (void*) _ll_val_sp_end);
    }
  }

  _ll_ASSERT_STACKS();
}


/**************************************************************************/
/* Probe the current message stack buffer */


void _ll_ar_push(int n)
{
  size_t size;
  ll_message *sb, *sp;

  size = n + 1 > ll_AR_SIZE ? n + 1 : ll_AR_SIZE;

  sb = ll_malloc(sizeof(sb[0]) * size);
  sp = sb + size;
  -- sp;

  if ( ll_STACK_DEBUG ) {
    fprintf(stderr, "\nll: _ll_ar_push:   SAVE: %p %p %p\n", (void*) _ll_ar_sp_base, (void*) _ll_ar_sp, (void*) _ll_ar_sp_bottom);
  }

  /* Save old stack pointers. */
  {
    void **vp = (void**) sp;
    
    assert(sizeof(*sp) >= sizeof(*vp) * 5);
    vp[0] = ((void**) vp) + 1; /* GUARD. */
    vp[1] = (void*) _ll_ar_sp;
    vp[2] = (void*) _ll_ar_sp_base; 
    vp[3] = (void*) _ll_ar_sp_bottom;
    vp[4] = ((void**) vp) + 4; /* GUARD. */
  }

  /* Start using new stack buffer. */
  _ll_ar_sp = _ll_ar_sp_bottom = sp;
  _ll_ar_sp_base = sb;

  if ( ll_STACK_DEBUG ) {
    fprintf(stderr, "ll: _ll_ar_push: %6d: %p %p %p\n", n, (void*) _ll_ar_sp_base, (void*) _ll_ar_sp, (void*) _ll_ar_sp_bottom);
  }
  _ll_ASSERT_AR_STACK();
}


void _ll_ar_pop()
     /* pop the current stack buffer */
{
  void **vp = (void**) _ll_ar_sp_bottom;

  /* Restore old stack pointers. */
  _ll_ar_sp = vp[1];
  _ll_ar_sp_base = vp[2];
  _ll_ar_sp_bottom = vp[3];

  if ( ll_STACK_DEBUG ) {
    fprintf(stderr, "\nll: _ll_ar_pop: RESTORE: %p %p %p\n", _ll_ar_sp_base, _ll_ar_sp, _ll_ar_sp_bottom);
  }
  _ll_ASSERT_STACKS();
}


/**************************************************************************/
/* These routines save/restore all global stacks for catch and call/cc */


void _ll_get_stack_state(ll_tsa_stack_state *ss)
{
  /* This may not be the same as the stack buffer's
   * sp, if _ll_vs_push() is called before
   * _ll_set_stack_state().
   */
  ss->_val_sp           = ll_BOX_locative(_ll_val_sp);
  ss->_val_stack_buffer = ll_BOX_locative(_ll_val_stack_buffer);

  ss->_ar_sp         = ll_BOX_locative(_ll_ar_sp);
  ss->_ar_sp_base    = ll_BOX_locative(_ll_ar_sp_base);
  ss->_ar_sp_bottom  = ll_BOX_locative(_ll_ar_sp_bottom);

  /* Save fluid bindings */
  ss->_fluid_bindings = ll_g(_fluid_bindings);

  /* Save catch/throw */
  ss->_current_catch = ll_g(_current_catch);

}


void _ll_set_stack_state(const ll_tsa_stack_state *ss)
{
  ll_tsa_stack_buffer *sb;

  /* Get the value stack buffer that we need to restore. */
  sb = (void*) ll_UNBOX_locative(ss->_val_stack_buffer);
  /* Restore to it. */
  _ll_val_stack_buffer_switch(sb);
  /* Restore the sp into the stack buffer as saved. */
  _ll_val_sp = (void*) ll_UNBOX_locative(ss->_val_sp);

  /* Handle message stack. */
  _ll_ar_sp  = (void*) ll_UNBOX_locative(ss->_ar_sp);
  _ll_ar_sp_base  = (void*) ll_UNBOX_locative(ss->_ar_sp_base);
  _ll_ar_sp_bottom  = (void*) ll_UNBOX_locative(ss->_ar_sp_bottom);

  _ll_ASSERT_STACKS();

  /* Restore fluid bindings */
  ll_set_g(_fluid_bindings, ss->_fluid_bindings);

  /* Restore catch/throw */
  ll_set_g(_current_catch, ss->_current_catch);

}


/**************************************************************************/


ll_INIT(stack, 110, "message and value stacks")
{
  _ll_val_stack_buffer = 0; /* There is no buffer before this one. */
  _ll_ar_stack_buffer = 0; /* There is no buffer before this one. */

  /* Value stack: allocate stack buffer and push dummy element */
  _ll_vs_push(1);

  /* Value stack: Push dummy element */
  _ll_PUSH(ll_undef);

  /* Message stack: allocate stack buffer and push dummy element. */
  _ll_ar_push(1);
  _ll_ar_sp --;
  _ll_ar_sp->super_object._isa = ll_type(message);
  _ll_ar_sp->_type = ll_type(undefined);
  _ll_ar_sp->_type_offset = ll_make_fixnum(0);
  _ll_ar_sp->_op = ll_o(make);
  _ll_ar_sp->_meth = ll_undef; /* (make <interpreter>) */
  _ll_ar_sp->_argc = 0;
  _ll_ar_sp->_db_at_rtn = ll_f;

  /* Message stack: initialize dummy element. */
  _ll_ar_sp->_argv = (ll_v *) _ll_val_sp;
  _ll_ar_sp->_argv_sb = (void *) _ll_val_stack_buffer;

  /* Note: fluid binding stack is initialized after nil object. */

  return 0;
}


/**************************************************************************/


#ifdef _ll_PUSH
#undef _ll_PUSH
#endif

#if 0

void _ll_PUSH(ll_v x)
{
  __ll_PUSH(x);
  _ll_ASSERT_STACKS();
  /* 
     fprintf(stderr, "_ll_PUSH(): %p %p %p\n", _ll_val_sp_base, _ll_val_sp, _ll_val_sp_bottom );
  */
}
#endif

#ifdef _ll_POP
#undef _ll_POP
#endif


#if 0
ll_v _ll_POP()
{
  ll_v x;

  /*
    fprintf(stderr, "_ll_POP():  %p %p %p\n", _ll_val_sp_base, _ll_val_sp, _ll_val_sp_bottom );
  */
  x = __ll_POP();
  _ll_ASSERT_STACKS();
  return x;
}
#endif


/**************************************************************************/
