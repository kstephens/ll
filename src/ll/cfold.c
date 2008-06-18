#include "ll.h"
#include "bcompile.h"

/*
 * Constant expression folding.
 *
 * Is disabled by default; standard Scheme test are failing.
 */
/**********************************************************************/

#define IR ll_ARG_1

ll_define_primitive(object, _ir_constantQ, _2(self, ir), _0())
{
  ll_call_tail(ll_o(constantQ), _1(ll_SELF));
}
ll_define_primitive_end


/* Returns true if symbol's binding is readonly. */
ll_define_primitive(symbol, _ir_readonlyQ, _1(self), _1(no_side_effect, "#t"))
{
  ll_v x;

  x = ll_call(ll_o(_binding), _1(ll_SELF));
  if ( ll_unbox_boolean(x) ) {
    x = ll_call(ll_o(readonlyQ), _1(x));
  } else {
    x = ll_call(ll_o(property), _2(ll_SELF, ll_s(readonly)));
  }

  ll_return(x);
}
ll_define_primitive_end


ll_define_primitive(symbol, _ir_constantQ, _2(self, ir), _0())
{
  ll_v x;

  /* "if" is a synactic primitive and is never evaled. */
  if ( ll_EQ(ll_SELF, ll_s(if)) ) {
    x = ll_t;
  } else {
    ll_v s = ll_call(ll_o(_ir_var_scope), _2(IR, ll_SELF));
    ll_v type = var_type(s);

    /*
      ll_format(ll_undef, "%ir-constant? ~S => ~S\n", 2, ll_SELF, s);
    */

    /*
    ** If the symbol is bound to a global readonly value.
    ** get the value and quote it.
    */

    /* glo ref */
    if ( ll_EQ(type, ll_s(glo)) ) {
      x = ll_call(ll_o(_ir_readonlyQ), _1(ll_SELF));
    } else {
      x = ll_f;
    }
  }

  ll_return(x);
}
ll_define_primitive_end


ll_define_primitive(pair, _ir_constantQ, _2(self, ir), _1(no_side_effect, "#t"))
{
  /* Quoted forms are always constants. */
  if ( ll_EQ(ll_THIS->_car, ll_s(quote)) ) {
    ll_return(ll_t);
  }
  ll_return(ll_f);
}
ll_define_primitive_end


/**********************************************************************/

/*
** Begin folding expressions, by returning new expressions
** if constant folding can be applied.
*/

ll_define_primitive(object, _ir_const_fold, _2(self, ir), _1(no_side_effect, "#t"))
{
  /* Most expression objects cannot be folded. */ 
  ll_return(ll_SELF);
}
ll_define_primitive_end


ll_define_primitive(symbol, _ir_const_fold, _2(self, ir), _0())
{
  ll_v x = ll_call(ll_o(_ir_var_scope), _2(IR, ll_SELF));
  ll_v type = var_type(x);

  /*
  ** If the symbol is bound to a global readonly value.
  ** get the value and quote it.
  */

  /* glo ref */
  if ( ll_EQ(type, ll_s(glo)) ) {
    x = ll_call(ll_o(_ir_readonlyQ), _1(ll_SELF));
    if ( ll_unbox_boolean(x) ) {
      x = ll_call(ll_o(_get), _1(ll_SELF));
      /* 
	 ll_format(ll_undef, "%ir-const-fold glo ~S -> ~S\n", 2, ll_SELF, x);
      */
      ll_return(ll_quote(x));
    }
  }

  /* Otherwise leave it alone. */
  ll_return(ll_SELF);
}
ll_define_primitive_end


ll_define_primitive(pair, _ir_const_fold, _2(self, ir), _0())
{
  ll_v result = ll_SELF;
  int all_consts = 1;

  /*
    ll_format(ll_undef, "%ir-const-fold ~S =>\n", 1, self); 
  */

  /*
  ** Special case:
  ** Quoted forms are always left alone.
  */
  if ( ll_EQ(ll_THIS->_car, ll_s(quote)) ) {
    goto result;
  }
  
  /*
  ** Special case:
  ** Skip over var in (define var ...) and (set! var ...) and
  ** attempt to fold ... constants.
  */
  else if ( ll_EQ(ll_THIS->_car, ll_s(define)) || ll_EQ(ll_THIS->_car, ll_s(setE)) ) {
    ll_v values = ll_cdr(ll_THIS->_cdr);
    ll_LIST_LOOP_REF(values, x);
    {
      *x = ll_call(ll_o(_ir_const_fold), _2(*x, IR));
    }
    ll_LIST_LOOP_END;

    goto result;
  }

  /* Handle (if <cond> <true> <consequent>) */
  /*
    Test case:

    (if (car '(#t . #f)) "true" "false") ; => "true"
    (if (cdr '(#t . #f)) "true" "false") ; => "false"
    (if (cons #t #f) "true" "false") ; => "true"
  */
  else if ( ll_EQ(ll_THIS->_car, ll_s(if)) ) {
    ll_v x, cond_expr, cond, tbranch;

    x = ll_THIS->_cdr;
    cond_expr = ll_car(x); x = ll_cdr(x);
    tbranch = ll_car(x); x = ll_cdr(x);

    /* Attempt to fold <cond> */
    cond = ll_call(ll_o(_ir_const_fold), _2(cond_expr, IR));
    
    /*
      ll_format(ll_undef, "%ir-const-fold if: cond ~S => ~S\n", 2, cond_expr, cond); 
    */

    /* If <cond> is a constant, */
    if ( ll_unbox_boolean(ll_call(ll_o(_ir_constantQ), _2(cond, IR))) ) {
      /* Evaluate it. */
      cond = ll_call(ll_o(eval_no_const_fold), _1(cond));

      if ( ll_unbox_boolean(cond) ) {
	/*
	** If cond is true,,
	** (if <cond> <tbranch> ...) => <tbranch>
	*/
	result = tbranch;
	/*
	  ll_format(ll_undef, "%ir-const-fold if: ~S => true => ~S\n",
	  2, ll_SELF, result);
	*/
      } else if ( ! ll_nullQ(x) ) {
	/*
	** If cond is false,
	** (if <cond> <tbranch> <fbranch>) => <fbranch>
	*/
	result = ll_car(x);
	/*
	  ll_format(ll_undef, "%ir-const-fold if: ~S => false => ~S\n", 2, ll_SELF, result); 
	*/
      } else {
	/*
	** If cond is false,
	** (if <cond> <tbranch>) => <unspecified>
	*/
	result = ll_unspec;
	/*
	  ll_format(ll_undef, "%ir-const-fold if: ~S => unspec => ~S\n", 2, ll_SELF, result); 
	*/
      }
    }

    goto result;
    /* FALL THROUGH */
  }

  /*
  ** General case:
  ** Attempt to fold an operation and arguments.
  */
  ll_LIST_LOOP_REF(ll_SELF, x);
  {
    ll_v cv;

    cv = ll_call(ll_o(_ir_const_fold), _2(*x, IR));
    /* Was the result of the constant fold a constant? */
    if ( ! ll_unbox_boolean(ll_call(ll_o(_ir_constantQ), _2(cv, IR))) ) {
      /* Not all the expr elements were constants. */
      all_consts = 0;
    }
    *x = cv;
  }
  ll_LIST_LOOP_END;

  /*
    ll_format(ll_undef, "  %ir-const-fold mapped => ~S\n", 1, ll_SELF); 
  */

  {
    ll_v x;
    ll_v op, args;
    ll_v type, meth, no_side_effect;

    if ( ! all_consts ) {
      /* We need to compile it further, because the expression could not be entirely folded. */
      goto result;
    }

    /* All are constants */

    /* Evaluate the op expression. */
    x = ll_THIS->_car;
    op = ll_call(ll_o(eval_no_const_fold), _1(x));

    /*
      ll_format(ll_undef, "%ir-const-fold ~S: op ~S => ~S\n", 3, self, x, op);
    */

    /* op is not an operation! */
    if ( ! (ll_ISA_ref(op) && 
	    ll_unbox_boolean(ll_call(ll_o(is_aQ), _2(op, ll_type(operation))))
	    )
	 ) {
      /*
	ll_format(ll_undef, "%ir-const-fold ~S: NOT op ~S => ~S\n", 3,
	ll_SELF, x, op);
      */
      goto result;
    }

    args = ll_THIS->_cdr;

    /* lookup op */
    if ( ll_nullQ(args) ) {
      type = ll_type(object);
    } else {
      ll_v rcvr;

      /* Eval the rcvr, so we can get an actual type. */
      x = ll_car(args);
      rcvr = ll_call(ll_o(eval_no_const_fold), _1(x));
      /*
	ll_format(ll_undef, "%ir-const-fold ~S: rcvr ~S => ~S\n", 3, self, x, rcvr);
      */
      type = ll_TYPE(rcvr);
    }

    meth = ll_call(ll_o(lookup), _2(type, op));
    /*
    ll_format(ll_undef, "%ir-const-fold ~S: type ~S, op ~S => meth ~S\n", 4, ll_SELF, type, op, meth);
    */
    
    /* If the method has no side-effect, evaluate it now. */
    if ( ll_unbox_boolean(meth) ) {
      no_side_effect = ll_call(ll_o(property), _2(meth, ll_s(no_side_effect)));
      if ( ll_unbox_boolean(no_side_effect) ) {
        /*
	ll_format(ll_undef, "%ir-const-fold ~S: ~S no-side-effect => ~S\n", 3, ll_SELF, op, meth);
	*/
	result = ll_call(ll_o(eval_no_const_fold), _1(ll_SELF));

	result = ll_quote(result);
      }
    }
  }

 result:
  /* 
     ll_format(ll_undef, "%ir-const-fold ~S: result ~S\n", 2, self, result); 
  */
  ll_return(result);
}
ll_define_primitive_end


ll_define_primitive(_ir, _ir_const_fold, _2(self, ir), _0())
{
  ll_LIST_LOOP_REF(ll_THIS->_body, x);
  {
    *x = ll_call(ll_o(_ir_const_fold), _2(*x, ll_SELF));
  }
  ll_LIST_LOOP_END;

  ll_return(ll_SELF);
}
ll_define_primitive_end


#undef IR

/***************************************************************************/

