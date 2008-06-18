#ifndef __rcs_id__
#ifndef __rcs_id_ll_char_c__
#define __rcs_id_ll_char_c__
static const char __rcs_id_ll_char_c[] = "$Id: callcc.c,v 1.5 2008/01/06 18:36:33 stephens Exp $";
#endif
#endif /* __rcs_id__ */

#include "ll.h"
#include "ccont/ccont.h"


ll_define_primitive(operation, call_with_current_continuation, _1(proc), _0())
{
  ll_call_tail(ll_o(make), _2(ll_type(continuation_method), ll_ARG_0));
}
ll_define_primitive_end


ll_define_primitive(object, _continuation_apply, _1(result), _0()) 
{
  ll_tsa_continuation_method *cc = ll_THIS_ISA(continuation_method, ll_AR_METH);

  /* Box up the result in the continuation. */
  CCont_call(cc->_cc, (void *) ll_ARG_0); /* unportable! */

  /* NOTREACHED */
  ll_abort();
}
ll_define_primitive_end


ll_define_primitive(continuation_method, initialize, _2(cc, proc), _0())
{
  ll_v op = ll_undef;
  ll_v result = ll_undef;

  ll_call_super(ll_o(initialize), ll_f, _1(ll_ARG_0));

  CCont_begin(cc);
  {
    /* Invoking this method will use <object>:%contination_apply below. */
    ll_THIS_ISA(method, ll_SELF)->_func = 
      _ll_prim_func_name(object, _continuation_apply);

    /* Save the CCont in this method. */
    CCont_assign(ll_THIS->_cc, cc);

    /* Save the stack state. */
    _ll_get_stack_state(&ll_THIS->super_stack_state);

    /* Make an anonymous <operation>. */
    op = _ll_make_operation();

    /* Add this method to the operation. */
    _ll_add_method(ll_type(object), op, ll_SELF); 

    /* Call the proc with the operation. */
    result = ll_call(ll_ARG_1, _1(op));
  } CCont_called(); {
    /* CCont_call() called. */

    /* Save the stack state. */
    _ll_set_stack_state(&ll_THIS->super_stack_state);

    /* Return result. */
    result = (ll_v) CCont_args(cc); /* unportable! */
  } 
  CCont_end();

  ll_return(result);
}
ll_define_primitive_end


/* FIXME!! */
ll_define_primitive(continuation_method, _finalize, _1(cc), _0())
{
  CCont_free(ll_THIS->_cc);
}
ll_define_primitive_end


/*
TEST

(+ 1 
  (call-with-current-continuation 
    (lambda (cc)
      (cc 5)
      7)
      ))
;; => 6

(define c #f)
(+ 1
    (call-with-current-continuation
      (lambda (cc)
        (set! c cc)
	2
      )
    )
  )
;; => 3
(c 10)
;; => 10

*/


