/***************************************************************************/

/* 1st pass:
** 1. Translate (%method (<formals> . <slots>) . <body>) into ir.
** 2. Determine variable scope.
** 3. Allocate locals.
*/


/***************************************************************************/

#include "bcompile.h"

#if 0
#define DEBUG_PRINTF(X) fprintf X
#else
#define DEBUG_PRINTF(X) (void) 0
#endif

/***************************************************************************/


ll_define_primitive(_ir, _ir_compile1_body, _1(ir), _0())
{
  ll_v newbody = ll_nil, *n = &newbody;

  DEBUG_PRINTF((stderr, "("));

  ll_LIST_LOOP(ll_THIS->_body, x);
  {
     /*
     ** Remember the number of locals for us here since
     ** each car-pos lambda can share parts of the local variable space.
     */
     // ll_v n_locals_save = ll_THIS->_max_n_locals;

     *n = ll_cons(ll_call(ll_o(_ir_compile1), _3(x, ll_SELF, ll_f)), ll_nil);
     n = &ll_CDR(*n);

     // ll_THIS->_n_locals = n_locals_save;
     ll_write_barrier_SELF();
  }
  ll_LIST_LOOP_END;

  DEBUG_PRINTF((stderr, ")"));

  ll_THIS->_body = newbody;
  ll_write_barrier_SELF();
}
ll_define_primitive_end


#define SELF ll_SELF
#define IR ll_ARG_1
#define CAR_POSQ ll_ARG_2


ll_define_primitive(constant, _ir_compile1, _3(self, ir, car_posQ), _0())
{
  ll_assert_ref(IR);
  ll_return(ll_SELF);
}
ll_define_primitive_end


ll_define_primitive(object, _ir_compile1, _3(self, ir, car_posQ), _0())
{
  ll_assert_ref(IR);
  _ll_error(ll_ee(syntax), 1, ll_s(form), ll_SELF);
  ll_return(ll_SELF);
}
ll_define_primitive_end


ll_define_primitive(_ir, _ir_compile1, _3(self, ir, car_posQ), _0())
{
  ll_assert_ref(IR);
  ll_return(ll_SELF);
}
ll_define_primitive_end


ll_define_primitive(symbol, _ir_compile1, _3(self, ir, car_posQ), _0())
{
  ll_assert_ref(IR);
  /* induce variable scoping */
  ll_call(ll_o(_ir_var_scope), _2(IR, SELF));
  ll_return(ll_SELF);
}
ll_define_primitive_end


/* R5RS 5.2.2. Internal Definitions */
static
ll_v _ll_scan_for_body_defines(ll_v body)
{
#define APPEND_BEGIN(x)  ll_v x = ll_nil, *x##p = &x
#define APPEND(x,V) (x##p = &ll_CDR(*x##p = ll_cons((V), ll_nil)))
  
  APPEND_BEGIN(vars);
  APPEND_BEGIN(inits);
  APPEND_BEGIN(sets);
  
  /* Scan for beginning (define ...) forms */
  ll_LIST_LOOP(body, x);
  {
    /* Is it a (define <var> <value>) form? */
    if ( ll_pairQ(x) && ll_eqQ(ll_car(x), ll_s(define)) ) {
      ll_v var = ll_cadr(x);
      ll_v init = ll_caddr(x);
      ll_v set = ll_listn(3, ll_s(setE), var, init);
      
      init = ll_cons(ll_s(quote), ll_cons(ll_undef, ll_nil));

      if ( 0 ) {
	ll_format(ll_undef, "body define: ~S\n", 1, x);
	ll_format(ll_undef, "  ~S ~S ~S\n", 3, var, set, init);
      }

      APPEND(vars, var);
      APPEND(inits, init);
      APPEND(sets, set);
      
      /* Skip it in the body */
      body = ll_cdr(body);
    } else {
      /* Stop scanning for (define ...) */
      break;
    }
  }
  ll_LIST_LOOP_END;

#undef APPEND_BEGIN
#undef APPEND
  
  /* Did we get any (define ...)? */
  if ( ! ll_nullQ(vars) ) {
    ll_v x;
    
    /*
      (lambda <formals> (define <x> <v>) ... . <body) =>
      (lambda <formals> (letrec ((<x> <v>) ...)  . <body>))
    */
    
    /* Put the real body after the set! expressions */
    *setsp = body;
    
    x = ll_cons(ll_cons(ll_s(lambda), ll_cons(vars, sets)), inits);
    
    /* The new body is a letrec */
    body = ll_cons(x, ll_nil);

    if ( 0 ) {
      ll_format(ll_undef, "body defines=~S\n", 1, body);
    }
  }
  
  return body;
}


ll_define_primitive(pair, _ir_compile1, _3(self, ir, car_posQ), _0())
{
  ll_v car = ll_THIS->_car;
  ll_v x = ll_THIS->_cdr;
  ll_v expr;

  DEBUG_PRINTF((stderr, "."));

  ll_assert_ref(IR);

  if ( 0 ) {
    ll_call(ll_o(print_frame), _1(ll_AR));
  }

  if ( _ll_bcc_1 ) {
    ll_format(ll_undef, "\n\ncompile-1:  \n", 0);
    ll_call(ll_o(_write_ir), _2(ll_ARG_1, ll_undef));
  }
  if ( ll_EQ(car, ll_s(_bcc_1)) ) {
    ll_format(ll_undef, "\n\ncompile-1:  \n", 0);
    ll_call(ll_o(_write_ir), _2(ll_ARG_1, ll_undef));
    ll_return(ll_undef);
  }
  if ( ll_EQ(car, ll_s(_bcc_emit)) ) {
    ll_DEBUG_SET(compile_emit, ll_make_fixnum(1));
    ll_return(ll_undef);
  }

  /* ll_format(ll_undef, "%ir-compile1 ~S\n", 1, ll_SELF); */

  /* Handle special forms. */
  if ( ll_EQ(car, ll_s(quote)) ) {
    /* (quote ...) */

    expr = ll_SELF;
  } else if ( ll_EQ(car, ll_s(_make_locative)) ) {
    /* (%make-locative (quote <var>)) */

    ll_v var = ll_car(ll_cdr(ll_car(x)));

    if ( ll_unbox_boolean(ll_call(ll_o(symbolQ), _1(var))) ) {
      /* Mark the var as closed over, because we need a locative */
      ll_call(ll_o(_ir_var_scope), _3(IR, var, ll_t));
    } else {
      _ll_error(ll_ee(syntax), 1, ll_s(form), ll_SELF);
    }

    ll_assert_ref(IR);
  

    expr = ll_SELF;
  } else if ( ll_EQ(car, ll_s(lambda)) ) {
    /* (lambda <formals> . <body>) -> 
       (%add-method <object> (make <operation>) (%method (<formals>) . <body>)) */

    ll_v slots = ll_nil;
    ll_v formals = ll_car(x);
    ll_v body = ll_cdr(x);
    ll_v newir;
 
    body = _ll_scan_for_body_defines(body);

    newir = ll_call(ll_o(make), _6(ll_type(_ir), body, formals, slots, IR, CAR_POSQ));

    ll_assert_ref(IR);

    /* 
    ** If the lambda is in car-position. it will be inlined into the enclosing
    ** ir's code vector.
    */
    if ( ll_unbox_boolean(CAR_POSQ) ) {
      expr = newir;
    } else {
      /* Rewrite */
      /*
	(lambda <args> . <body>) ->
	  (%lambda <%ir>)

	  WAS:
	(%add-method <object> (make <operation>) <%ir>)
      */
	 

      expr = ll_listn(2,
		      ll_s(_lambda),
		      newir);

      /* 
      expr = ll_cons(ll_s(_add_method),
	     ll_cons(ll_s(LTobjectGT),
             ll_cons(ll_cons(ll_s(make), ll_cons(ll_s(LToperationGT), ll_nil)),
             ll_cons(newir,
                     ll_nil))));
      */

      ll_assert_ref(IR);
      /* Recompile */
      expr = ll_call(ll_o(_ir_compile1), _3(expr, IR, CAR_POSQ));
      // ll_format(ll_undef, "lambda : ~S => ~S\n", 2, ll_SELF, expr);
      ll_assert_ref(IR);
    }

  } else if ( ll_EQ(car, ll_s(_lambda)) ) {
    expr = ll_car(x);
    expr = ll_call(ll_o(_ir_compile1), _3(expr, IR, CAR_POSQ));
    expr = ll_listn(2, car, expr);
    // ll_format(ll_undef, "lambda : ~S => ~S\n", 2, ll_SELF, expr);
  } else if ( ll_EQ(car, ll_s(_method)) ) {
    /* (%method (<formals> . <slots>) . <body>) */
    ll_v fa = ll_car(x);
    ll_v body = ll_cdr(x);
    ll_v formals = ll_car(fa);
    ll_v slots = ll_cdr(fa);
    ll_v newir;

    body = _ll_scan_for_body_defines(body);
    ll_assert_ref(IR);

    /* method constructors are never car-pos */
    newir = ll_call(ll_o(make), _6(ll_type(_ir), body, formals, slots, IR, ll_f));
    ll_assert_ref(IR);

    expr = newir;
  } else {
    /* (<op> . <formals>) */
    ll_v prefix = ll_undef;
    ll_v *n = &expr;
    ll_v op_in_car_pos = ll_t, args_in_car_pos = ll_f;
    int op_is_constant = 0;

    x = ll_SELF;

    if ( ll_EQ(car, ll_s(setE)) || ll_EQ(car, ll_s(define)) ) { 
      /* (set! <var> <value>) */
      /* (define <var> <value> ?<doc>?) */
      
      ll_v var = ll_car(ll_cdr(x));

      DEBUG_PRINTF((stderr, "\n 1 %s ", (char*) ll_po(var)));

      if ( 0 ) {
	ll_format(ll_undef, "scope: (~S ~S ...)\n", 2, car, var);
      }

      if ( ll_unbox_boolean(ll_call(ll_o(symbolQ), _1(var))) ) {
	/* Close over the var. */
	ll_call(ll_o(_ir_var_scope), _3(IR, var, ll_t));
	ll_assert_ref(IR);
	op_is_constant = 1;
	op_in_car_pos = ll_f;

	/* Use ({set!|define} ...) literally */
	prefix = car;
      } else {
	_ll_error(ll_ee(syntax), 1, ll_s(form), ll_SELF);
      }
    } else 
    if ( ll_EQ(car, ll_s(if)) ) {
      /* If: (if <op> <t> <f>) is in car-pos
      ** Then: <op> is not car_pos, but <t> and <f> are!
      */
      op_in_car_pos = ll_f;
      args_in_car_pos = CAR_POSQ;

      /* Use (if ...) literally */
      prefix = car;
    } else
    if ( ll_EQ(car, ll_s(super)) ) {
      /* (super <op> (<type>|#f) . <args>)
      */

      /* Use (super ...) literally */
      prefix = ll_s(super);
    }

    /* Handle prefix. */
    if ( ll_NE(prefix, ll_undef) ) {
      *n = ll_cons(prefix, ll_nil);
      n = &ll_CDR(*n);
      x = ll_cdr(x);
    }
    ll_assert_ref(IR);

    /* <op> is in car-position */
    {
      ll_v op = ll_car(x);

      if ( op_is_constant ) {
	if ( 0 ) {
	  ll_format(ll_undef, "op-constant: ~S\n", 1, op);
	}
      } else {
	op = ll_call(ll_o(_ir_compile1), _3(op, IR, op_in_car_pos));
      }
      *n = ll_cons(op, ll_nil);
      n = &ll_CDR(*n);
      x = ll_cdr(x);
      ll_assert_ref(IR);
    }

    /* <args> might be in car-position */
    ll_LIST_LOOP(x, q);
    {
      ll_v qc1 = ll_call(ll_o(_ir_compile1), _3(q, IR, args_in_car_pos));
      *n = ll_cons(qc1, ll_nil);
      n = &ll_CDR(*n);
      ll_assert_ref(IR);
    }
    ll_LIST_LOOP_END;
  }

  ll_return(expr);
}
ll_define_primitive_end


/**********************************************************************/

#undef SELF
#undef IR
#undef CAR_POSQ

/**********************************************************************/

