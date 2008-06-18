#ifndef __rcs_id__
#ifndef __rcs_id_ll_catch_c__
#define __rcs_id_ll_catch_c__
static const char __rcs_id_ll_catch_c[] = "$Id: catch.c,v 1.24 2008/05/26 07:48:08 stephens Exp $";
#endif
#endif /* __rcs_id__ */

#include "ll.h"

/*
<catch> is a subtype of <primitive>.  It provides the basics for error handling in ll.
A catch object is added to a specified type as a method using an anonymous operation.  This anonymous operation is later applied to an object of the specified type.  When the catch's operation is applied to an object of the type specified (this object is the "thrown value") for the catch,  execution resumes at the catch handle procedure or thrown value is returned from (catch). 

This allows catches to be created for specific error types to behandled by the proper <caught> bodies.

For example:

(catch <string>
  (lambda (the-catch) ; The catch body 
    (do-this)
    (do-that)
    (the-catch "throw this value")
    (this-is-never-done)
    )
  (lambda (the-value-thrown) ; The catch procedure
    (do-something the-value-thrown)))

The (catch <a-type> <body> ?<caught-body>) method
*/

void _ll_begin_catch(ll_v c)
{
  ll_tsa_catch *x = ll_THIS_ISA(catch, c);

  x->_previous_catch = ll_g(_current_catch);

  /* Remember current stack depths */
  _ll_get_stack_state(&x->super_stack_state);

  /* Make the catch valid */
  x->_validQ = ll_t;

  ll_set_g(_current_catch, c);
}


static
ll_v _ll_invalidate_catch(ll_v c)
{
  ll_tsa_catch *x = ll_THIS_ISA(catch, c);
  ll_v pc = x->_previous_catch;

  x->_previous_catch = ll_f;
  x->_value = ll_f;

  /* This is faster than it looks */
  _ll_remove_method(x->_type, x->_op);

  return pc;
}


ll_v  _ll_end_catch(ll_v c)
{
  ll_tsa_catch *x = ll_THIS_ISA(catch, c);
  ll_v value = x->_value;
  ll_v cc = ll_g(_current_catch);

  x->_validQ = ll_f;

  /* Invalidate every catch between the current catch
  ** and the catch we are longjmping to.
  */
  while ( ll_NE(cc, c) ) {
    /*
    ** If we've reached the bottom of the catch chain,
    **   something has been screwed up.
    */
    if ( ll_EQ(cc, ll_f) ) {
      _ll_error(ll_fe(catch), 
		2, 
		ll_s(catch), ll_f, 
		ll_s(reason), ll_s(catch_chain_corrupted));
      abort();
    }
    cc = _ll_invalidate_catch(cc);
  }

  /* Restore stack pointers */
  _ll_set_stack_state(&x->super_stack_state);

  /* Make cc prev catch the current catch */
  ll_set_g(_current_catch, _ll_invalidate_catch(c));

  return value;
}


/*
** This should never be called directly.
*/
ll_define_primitive(object, _catch_apply, __0(value), _0())
{
  ll_v the_catch = ll_AR_METH;
  ll_tsa_catch *x = ll_THIS_ISA(catch, the_catch);
  ll_v validQ = x->_validQ;
  ll_v value = ll_ARGC ? ll_ARG_0 : ll_unspec;

  /*
  ** Check if the catch has been thrown to before.
  */
  x->_validQ = ll_f;

  if ( ll_EQ(validQ, ll_f) ) {
    ll_return(_ll_error(ll_ee(catch), 2, 
			ll_s(catch), the_catch, 
			ll_s(reason), ll_s(already_thrown_to)));
  }

  x->_value = value;
  longjmp(x->__jb, 1);
}
ll_define_primitive_end


/*
 */

ll_define_primitive(type, catch, __1(body, caught), _0())
{
  ll_v rtn = ll_undef;

  ll_CATCH_BEGIN(ll_SELF, c);
  {
    /* call the body with the catch */
    rtn = ll_call(ll_ARG_1, _1(c));
  }
  ll_CATCH_VALUE(v);
  {
  /* if we have a catch body:  */
    if ( ll_ARGC >= 3 ) {
      /* call it with the caught value */
      ll_call_tail(ll_ARG_2, _1(v));
    } else {
      /* rtn the caught value */
      rtn = v;
    }
  }
  ll_CATCH_END;

  ll_return(rtn);
}
ll_define_primitive_end


ll_v _ll_make_catch(ll_v type, ll_v *xp)
{
  ll_v x = _ll_allocate_type(ll_type(catch));
  ll_tsa_catch *t = ll_THIS_ISA(catch, x);

  /* Copy <object>::%catch-apply */
  *(ll_tsa_primitive*)t = *ll_THIS_ISA(primitive, ll_p(object, _catch_apply));

  ll_SLOTS(x)[0] = ll_type(catch); /* Fix isa */

  /* Remember what type and op this catch was associated with. */
  t->_type = type;
  t->_op = _ll_make_operation();
  t->_value = ll_undef; /* No value yet! */
  t->_validQ = ll_f; /* Not valid until setjmp() */

  /* This is faster than it looks. */
  _ll_add_method(t->_type, t->_op, x);

  *xp = x;
  return t->_op;
}


ll_INIT(catch, 120, "catch")
{
  ll_set_g(_current_catch, ll_f);

  return 0;
}
