#ifndef __rcs_id__
#ifndef __rcs_id_ll_macro_c__
#define __rcs_id_ll_macro_c__
static const char __rcs_id_ll_macro_c[] = "$Id: macro.c,v 1.20 2007/12/23 20:09:45 stephens Exp $";
#endif
#endif /* __rcs_id__ */


#include "ll.h"
#include "call_int.h"


/*********************************************************/
/* A simple macro expander */


ll_define_primitive(object,macro_expand_1, _1(obj), _1(no_side_effect,"#t"))
{
  ll_return(ll_SELF);
}
ll_define_primitive_end


/*********************************************************/


ll_define_primitive(pair,macro_expand_list, _1(obj), _1(no_side_effect,"#t"))
{
  ll_return(ll_cons(ll_call(ll_o(macro_expand), _1(ll_THIS->_car)),
		    ll_call(ll_o(macro_expand_list), _1(ll_THIS->_cdr))));
}
ll_define_primitive_end


ll_define_primitive(null,macro_expand_list, _1(obj), _1(no_side_effect,"#t"))
{
  ll_return(ll_SELF);
}
ll_define_primitive_end


/*********************************************************/


ll_define_primitive(pair,macro_expand_1, _1(obj), _1(no_side_effect,"#t"))
{
  ll_v macro;

  /* Special forms */

  if ( ll_EQ(ll_THIS->_car, ll_s(quote)) ) {
    /* (quote x) => (quote x) */
    ll_return(ll_SELF);
  } else
  
  if (  ((ll_EQ(ll_THIS->_car, ll_s(setE)) || ll_EQ(ll_THIS->_car, ll_s(define))) && ll_unbox_boolean(ll_call(ll_o(symbolQ), _1(ll_car(ll_THIS->_cdr)))))
       || ll_EQ(ll_THIS->_car, ll_s(lambda)) 
       || ll_EQ(ll_THIS->_car, ll_s(_method)) ) {
    /* (set! <var> ...) */
    /* (define <var> ...) */
    /* (lambda <formals> . <body>) 
       => `(lambda <formals> . ,(macro-expand-list <body>)) */
    /* (%method (<formals> . <slots>) . <body>)
       => `(%method (<formals> . <slots>) . ,(macro-expand-list <body>)) */

    ll_v x = ll_THIS->_cdr;

    ll_return(ll_cons(ll_THIS->_car, 
		      ll_cons(ll_car(x),
			      ll_call(ll_o(macro_expand_list), _1(ll_cdr(x))))));
  }

  /* Look up a macro */
  if ( ll_unbox_boolean(ll_call(ll_o(symbolQ), _1(ll_THIS->_car))) ) {
    macro = ll_call(ll_o(_macro), _2(ll_call(ll_o(SenvironmentS), _0()), ll_THIS->_car));
    /* Does the first arg respond to the macro? */
    if ( ll_unbox_boolean(macro) ) {
      ll_v t;

      /* Determine type of macro rcvr. */
      t = ll_nullQ(ll_THIS->_cdr) ? ll_type(object) : ll_TYPE(ll_car(ll_THIS->_cdr));

      /* Do a lookup. */
      t = ll_call(ll_o(lookup), _2(t, macro));

      /* If macro rcvr doesn't respond to macro op, don't call it. */
      if ( ! ll_unbox_boolean(t) ) {
	macro = ll_f;
      }
    }
  } else {
    macro = ll_f;
  }

  if ( ! ll_unbox_boolean(macro) ) {
    /* If it's not a macro, expand all the items in the sexpr. */
    ll_call_tail(ll_o(macro_expand_list), _1(ll_SELF));
  } else {
    /* If it's a macro, create a argument list of all the items */
    /* What is our argument length? */
    size_t l = 0;

    /* Find out the size of the tmp array, so we can probe the stack. */
    {
      ll_v x = ll_THIS->_cdr;

      for ( ; ! ll_nullQ(x); x = ll_cdr(x) ) {
	l ++;
      }
      _ll_VS_PROBE(l);
    }

    /* Map the list into the tmp array */
    {
      ll_v x = ll_THIS->_cdr;

      for ( ; ! ll_nullQ(x); x = ll_cdr(x) ) {
	_ll_PUSH(ll_car(x));
      }
    }

    /* Reverse */
    {
      ll_v *v = (ll_v*) _ll_val_sp;
      int i, j;
      
      for ( i = l, j = 0; -- i > j; j ++ ) {
	ll_v t = v[i];
	v[i] = v[j];
	v[j] = t;
      }
    }
    
    /* Apply the macro to the arguments */
    /* Do a tail call with the current stack */
    _ll_call_tailv(macro, l);
  }
}
ll_define_primitive_end


/*********************************************************/


ll_define_primitive(object,macro_expand, _1(obj), _1(no_side_effect,"#t"))
{
  ll_v x, y = ll_SELF;

  /*
    (macro-expand x) calls 
    (let ((y (macro-expand-1 x)))
    (set! x y))
    until (equal? x y)
  */
		   
  // ll_format(ll_undef, "macro-expand: ~S\n", 1, y);

  do {
    x = y;
    y = ll_call(ll_o(macro_expand_1), _1(x));
  } while ( ! ll_equalQ(x, y) );

  // ll_format(ll_undef, "macro-expand: RESULT: ~S => ~S\n", 2, ll_SELF, y);

  ll_return(y);
}
ll_define_primitive_end


/*********************************************************/
