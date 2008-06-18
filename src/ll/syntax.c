#ifndef __rcs_id__
#ifndef __rcs_id_ll_syntax_c__
#define __rcs_id_ll_syntax_c__
static const char __rcs_id_ll_syntax_c[] = "$Id: syntax.c,v 1.22 2007/12/18 10:37:00 stephens Exp $";
#endif
#endif /* __rcs_id__ */

#include "ll.h"

/*********************************************************/

ll_v ll_quote(ll_v x)
{
  return ll_cons(ll_s(quote), ll_cons(x, ll_nil));
}

#define APPEND_BEGIN(x)  ll_v x = ll_nil, *x##p = (x##p = &x)
#define APPEND(x,V) (x##p = &ll_CDR(*x##p = ll_cons((V), ll_nil)))
#define INSERT(x,V) (x = ll_cons((V), x))

/**********************************************************************/
/* 4.2. Derived expression types */


/**********************************************************************/
/* 4.2.1. Conditionals */

ll_define_macro(pair, cond, __1(clause1, clause2))
{
  ll_v expr = ll_unspec, *t = &expr;
  size_t i;
  ll_v temp = ll_f;

  for ( i = 0; i < ll_ARGC; ) {
    ll_v clause = ll_ARGV[i ++];
    ll_v test = ll_car(clause);
    ll_v body = ll_cdr(clause);
    int need_test_value = 0;


    if ( i == ll_ARGC && ll_eqQ(test, ll_s(else)) ) {
      /*
	(cond (else <body> ...)) ==>
	(begin <body> ...)
      */
      body = ll_cons(ll_s(begin), body);
      *t = ll_cons(body, ll_nil);
      break;
    } else if ( ll_nullQ(body) ) {
      /* (cond (<test>) ...) ==>
	 (begin
	 (set! <temp> <test>)
	 (if <temp> <test> (cond ...)))
      */
      if ( ! ll_unbox_boolean(temp) ) {
	temp = ll_call(ll_o(_gensym), _0());
      }
      need_test_value = 1;

      body = temp;
    } else if ( ll_eqQ(ll_car(body), ll_s(EQGT)) ) {
      /*
	(cond (<test> => <proc>) ...) ==>
	(begin
	(set! <temp> <test>)
	(if <temp> (<proc> <temp>) (cond ...)))
      */
      body = ll_cdr(body);

      if ( ! ll_unbox_boolean(temp) ) {
	temp = ll_call(ll_o(_gensym), _0());
      }      
      need_test_value = 1;
      
      body = ll_listn(2, ll_car(body), temp);
    } else {
      /* (cond (<test> <body> ...) ...) =>
	 (if <test> (begin <body> ...) (cond ...))
      */
      body = ll_cons(ll_s(begin), body);
    }


    {
      ll_v x;
      ll_v *next_t;

      if ( need_test_value ) {
	ll_v y;
	ll_v test2;

	if ( ! ll_unbox_boolean(temp) ) {
	  temp = ll_call(ll_o(_gensym), _0());
	}

	test2 = test;

	test = temp;

	x = ll_listn(3, ll_s(if), test, body);
	next_t = &ll_CDR(ll_CDR(ll_CDR(x)));

	y = ll_listn(3, 
		     ll_s(begin),
		     ll_listn(3, ll_s(setE), temp, test2),
		     x);

	x = y;
	
      } else {
	x = ll_listn(3, ll_s(if), test, body);
	next_t = &ll_CDR(ll_CDR(ll_CDR(x)));
      }

      *t = ll_cons(x, ll_nil);
      
      t = next_t;
    }
  }

  expr = ll_car(expr);

  if ( ll_unbox_boolean(temp) ) {
    expr = ll_listn(3,
		    ll_s(let),
		    ll_listn(1, ll_listn(2, temp, ll_undef)),
		    expr);
    
  }

  ll_return(expr);
}
ll_define_macro_end


ll_define_macro(object, _eqvQ_or, __1(value, test1))
{
  if ( ll_ARGC == 1 ) {
    ll_return(ll_f);
  } else if ( ll_ARGC == 2 ) {
    ll_return(ll_listn(3, ll_s(eqvQ), ll_ARG_0, ll_quote(ll_ARG_1)));
  } else {
    ll_return(ll_listn(4, 
		       ll_s(if), 
		       ll_listn(3, ll_s(eqvQ), ll_ARG_0, ll_quote(ll_ARG_1)),
		       ll_t,
		       ll_cons(ll_s(_eqvQ_or), ll_cons(ll_ARG_0, ll_listv(ll_ARGC - 2, ll_ARGV + 2)))));
  }
}
ll_define_macro_end


ll_define_macro(object, case, __2(key, clause1, clause2))
{
  ll_v expr = ll_unspec, *t = &expr;
  size_t i;
  ll_v temp;

  temp = ll_call(ll_o(_gensym), _0());

  for ( i = 1; i < ll_ARGC; ) {
    ll_v clause = ll_ARGV[i ++];
    ll_v test = ll_car(clause);
    ll_v body = ll_cdr(clause);

    if ( i == ll_ARGC && ll_eqQ(test, ll_s(else)) ) {
      /*
	(case (else <body> ...)) ==>
	(begin <body> ...)
      */
      body = ll_cons(ll_s(begin), body);
      *t = ll_cons(body, ll_nil);
      break;
    } else if ( ll_nullQ(body) ) {
      /* (case (<datum>) ...) ==>
	 #t
      */
      body = ll_unspec;
    } else {
      /* (case (<datum> <body> ...) ...) =>
	 (if <test> (begin <body> ...) (cond ...))
      */
      body = ll_cons(ll_s(begin), body);
    }

    {
      ll_v x;
      ll_v *next_t;

      test = ll_cons(ll_s(_eqvQ_or), ll_cons(temp, test));

      x = ll_listn(3, ll_s(if), test, body);
      next_t = &ll_CDR(ll_CDR(ll_CDR(x)));
      
      *t = ll_cons(x, ll_nil);
      
      t = next_t;
    }
  }

  expr = ll_car(expr);

  expr = ll_listn(3,
		  ll_s(let),
		  ll_listn(1, ll_listn(2, temp, ll_ARG_0)),
		  expr);
    

  ll_return(expr);
}
ll_define_macro_end


ll_define_macro(object, and, __0(exprs))
{
  if ( ll_ARGC == 0 ) {
    /* (and) => #t */

    ll_return(ll_t);
  } else if ( ll_ARGC == 1 ) {
    /* (and test) => test */

    ll_return(ll_ARG_0);
  } else {
    /* (and test1 test2 ...) => (if test1 (and test2 ...) #f)) */
    ll_return(ll_listn(4, ll_s(if),
		       ll_ARG_0,
		       ll_cons(ll_s(and), ll_listv(ll_ARGC - 1, ll_ARGV + 1)),
		       ll_f));
  }
}
ll_define_macro_end


ll_define_macro(object, or, __0(exprs))
{
  if ( ll_ARGC == 0 ) {
    /* (or) => #f */

    ll_return(ll_f);
  } else if ( ll_ARGC == 1 ) {
    /* (or test) => test */

    ll_return(ll_ARG_0);
  } else {
    /* (or test1 test2 ...) => 
         (let ((x test1))
           (if x x (or test2 ...))) */

    ll_v sym = ll_call(ll_o(_gensym), _0());

    ll_return(ll_listn(3, ll_s(let),
		       ll_listn(1, ll_listn(2, sym, ll_ARG_0)),
		       ll_listn(4, 
				ll_s(if),
				sym,
				sym,
				ll_cons(ll_s(or), ll_listv(ll_ARGC - 1, ll_ARGV + 1)))));
  }
}
ll_define_macro_end


/**********************************************************************/
/* R5RS 4.2.2. Binding constructs */


ll_define_macro(list, let, __1(bindings, body))
{
  APPEND_BEGIN(vars);
  APPEND_BEGIN(inits);
  ll_v body = ll_listv(ll_ARGC - 1, ll_ARGV + 1);

  /* Split vars and inits into separate lists */
  ll_LIST_LOOP(ll_ARG_0, x);
  {
    ll_v var = ll_car(x);
    ll_v init = ll_car(ll_cdr(x));
    APPEND(vars, var);
    APPEND(inits, init);
  }
  ll_LIST_LOOP_END;

  ll_return(ll_cons(ll_cons(ll_s(lambda), ll_cons(vars, body)), inits));
}
ll_define_macro_end


ll_define_macro(list, letS, __1(bindings, body))
{
  if ( ll_nullQ(ll_ARG_0) ) {
    /* (let* () ...) => (let () ...) */
    ll_return(ll_cons(ll_s(let), ll_listv(ll_ARGC, ll_ARGV)));
  } else {
    /* (let* ((name1 init1) (name2 init2) ...) body1 body2 ...) =>
         (let ((name1 init1))
           (let* ((name2 init2) ...)
             body1 body2 ...))
    */
    ll_v body = ll_listv(ll_ARGC - 1, ll_ARGV + 1);
    ll_return(ll_listn(3, 
		       ll_s(let), 
		       ll_listn(1, ll_car(ll_ARG_0)),
		       ll_cons(ll_s(letS), ll_cons(ll_cdr(ll_ARG_0), body))));
  }
}
ll_define_macro_end


ll_define_macro(list, letrec, __1(bindings, body))
{
  APPEND_BEGIN(vars);
  APPEND_BEGIN(inits);
  APPEND_BEGIN(body);

  /* Split vars and inits into separate lists */
  ll_LIST_LOOP(ll_ARG_0, x);
  {
    ll_v var = ll_car(x);
    ll_v init = ll_car(ll_cdr(x));
    APPEND(vars, var);
    APPEND(inits, ll_undef);
    APPEND(body, ll_listn(3, ll_s(setE), var, init));
  }
  ll_LIST_LOOP_END;

  *bodyp = ll_listv(ll_ARGC - 1, ll_ARGV + 1);

  ll_return(ll_cons(ll_cons(ll_s(lambda), ll_cons(vars, body)), inits));
}
ll_define_macro_end


/**********************************************************************/
/* R5RS 4.2.3. Sequencing */


ll_define_macro(object, begin, __0(body))
{
  /* (begin . <body>) => ((lambda () . <body>)) */
  if ( ll_ARGC == 0 ) {
    ll_return(ll_unspec);
  } else
  if ( ll_ARGC == 1 ) {
    ll_return(ll_ARG_0);
  } else {
    ll_return(ll_listn(1, ll_cons(ll_s(lambda),
				  ll_cons(ll_nil,
					  ll_listv(ll_ARGC, ll_ARGV)))));
  }
}
ll_define_macro_end


/**********************************************************************/
/* R5RS 4.2.4. Iteration */


ll_define_macro(list, do, __2(bindings, test, body))
{
  APPEND_BEGIN(nbindings);
  APPEND_BEGIN(steps);
  ll_v test = ll_car(ll_ARG_1);
  ll_v exprs = ll_cdr(ll_ARG_1);

  /* Split vars and inits into separate lists */
  ll_LIST_LOOP(ll_ARG_0, x);
  {
    ll_v var, init, step;

    var = ll_car(x); x = ll_cdr(x);
    init = ll_car(x); x  = ll_cdr(x);

    APPEND(nbindings, ll_listn(2, var, init));

    if ( ! ll_nullQ(x) ) {
      step = ll_car(x); x = ll_cdr(x);
      step = ll_listn(3, ll_s(setE), var, step);
      //ll_format(ll_undef, "do step ~S\n", 1, step);
      INSERT(steps, step);
    }
  }
  ll_LIST_LOOP_END;
  
  //ll_format(ll_undef, "do steps ~S\n", 1, steps);

  {
    ll_v loop_name, loop_def, body, x;
    ll_v branch, loop, loop_call;

    loop_name = ll_call(ll_o(_gensym), _0());
    
    loop_call = ll_cons(loop_name, ll_nil);

    body = ll_listv(ll_ARGC - 2, ll_ARGV + 2);
    
    body = ll_call(ll_o(append), _3(body, steps, ll_cons(loop_call, ll_nil)));    
    //ll_format(ll_undef, "do body + steps + loop_call ~S\n", 1, body);

    body = ll_cons(ll_s(begin), body);
    //ll_format(ll_undef, "do body ~S\n", 1, body);

    exprs = ll_cons(ll_s(begin), exprs);

    branch = ll_listn(4,
		      ll_s(if),
		      test,
		      exprs,
		      body);

    loop_def = ll_listn(3,
			ll_s(lambda),
			ll_nil,
			branch
			);

    loop_def = ll_listn(3,
			ll_s(setE),
			loop_name,
			loop_def);

    //ll_format(ll_undef, "do loop_def = ~S\n", 1, loop_def);

    loop = ll_listn(4,
		    ll_s(let),
		    ll_listn(1,
			     ll_listn(2, loop_name,
				      ll_f)),
		    loop_def,
		    loop_call);

    x = ll_listn(3,
		 ll_s(let),
		 nbindings,
		 loop);

    //ll_format(ll_undef, "do -> ~S\n", 1, x);

    ll_return(x);
  }
}
ll_define_macro_end


ll_define_macro(symbol, let, __2(variable, bindings, body))
{
  APPEND_BEGIN(vars);
  APPEND_BEGIN(inits);
  ll_v var = ll_ARG_0, body;

  /* Split vars and inits into separate lists */
  ll_LIST_LOOP(ll_ARG_1, x);
  {
    ll_v var = ll_car(x);
    ll_v init = ll_car(ll_cdr(x));
    x = ll_cdr(x);
    APPEND(vars, var);
    APPEND(inits, init);
  }
  ll_LIST_LOOP_END;

  body = ll_listv(ll_ARGC - 2, ll_ARGV + 2);

  {
    ll_v x;
    ll_v lambda;

    lambda = ll_cons(ll_s(lambda), ll_cons(vars, body));

    x = ll_listn(4,
		 ll_s(let),
		 ll_listn(1, ll_listn(2, var, ll_undef)),
		 ll_listn(3, ll_s(setE), var, lambda),
		 ll_cons(var, inits));


    ll_return(x);
  }
}
ll_define_macro_end


/**********************************************************************/
/* R5RS 4.2.5. Delayed evaluation */

  /* IMPLEMENT: Delay */


/**********************************************************************/
/* R5RS 4.2.6. Quasiquotation */


ll_define_macro(object, quasiquote, _1(template))
{
  ll_call_tail(ll_o(_quasiquote_expand), _2(ll_SELF, ll_make_fixnum(0)));
}
ll_define_macro_end


ll_define_primitive(object, _quasiquote_expand, _2(template, level), _1(no_side_effect,"#t"))
{
  ll_return(ll_quote(ll_SELF));
}
ll_define_primitive_end


ll_define_primitive(constant, _quasiquote_expand, _2(template, level), _1(no_side_effect,"#t"))
{
  ll_return(ll_SELF);
}
ll_define_primitive_end


ll_define_primitive(pair, _quasiquote_expand, _2(template, level), _1(no_side_effect,"#t"))
{
  ll_v level = ll_ARG_1;

  if ( ll_EQ(ll_THIS->_car, ll_s(quasiquote)) ) {
    level = ll__ADD(level, ll_make_fixnum(1));
  } else if ( ll_EQ(ll_THIS->_car, ll_s(unquote)) ) {
    level = ll__SUB(level, ll_make_fixnum(1));
  } else if ( ll_EQ(ll_THIS->_car, ll_s(unquote_splicing)) ) {
    level = ll__SUB(level, ll_make_fixnum(1));
  }

  if ( ll_eqvQ(ll_ARG_1, ll_make_fixnum(0)) ) {
    if ( ll_EQ(ll_THIS->_car, ll_s(unquote)) ) {
      /* `,<x> -> <x> */

      ll_return(ll_car(ll_THIS->_cdr));
    } else if ( ll_pairQ(ll_THIS->_car) && ll_eqQ(ll_car(ll_THIS->_car), ll_s(unquote_splicing)) ) {
      /* `((unquote-splicing <x>) . rest) */

      ll_v x = ll_car(ll_cdr(ll_THIS->_car));

      
      ll_return(ll_listn(3, 
			 ll_s(append),
			 x,
			 ll_call(ll_o(_quasiquote_expand), _2(ll_THIS->_cdr, ll_ARG_1))));
    }      
  }

  ll_return(ll_listn(3, 
		     ll_s(cons),
		     ll_call(ll_o(_quasiquote_expand), _2(ll_THIS->_car, level)),
		     ll_call(ll_o(_quasiquote_expand), _2(ll_THIS->_cdr, level))));
}
ll_define_macro_end


ll_define_primitive(vector, _quasiquote_expand, _2(template, level), _1(no_side_effect,"#t"))
{
  ll_v x;

  x = ll_call(ll_o(vector__list), _1(ll_SELF));
  x = ll_call(ll_o(_quasiquote_expand), _2(x, ll_ARG_1));
  ll_return(ll_listn(2, ll_s(list__vector), x));
}
ll_define_macro_end



/*********************************************************/
/* R5RS 5.2. Definitions  */


ll_define_macro(pair, define, __1(formals, body))
{
  /* (define (<sym> . <formals>) . <body>) => (define <sym> (lambda <formals> . <body>) */
  ll_v sym = ll_car(ll_ARG_0);
  ll_v formals = ll_cdr(ll_ARG_0);
  ll_v body = ll_listv(ll_ARGC - 1, ll_ARGV + 1);
  ll_return(ll_listn(3,
		     ll_s(define), 
		     sym, 
		       ll_cons(ll_s(lambda),
		       ll_cons(formals,
		               body))));
}
ll_define_macro_end


/*********************************************************/
/* Oaklisp settable operations 
 * (set! (<op> . <args>) <value>)
 */


ll_define_macro(pair, setE, _2(expr, value))
{
  /* (set! (<op> . <args>) <value>)
     => ((setter <op>) . <args> <value>)
  */

  /* Macro-expand (<op> . <args>) first to handle things like
  ** (set! (fluid x) v)
     => ((setter %fluid) 'x v)
  */
  ll_v expr = ll_call(ll_o(macro_expand), _1(ll_SELF));
  ll_v op = ll_car(expr);
  ll_v args = ll_cdr(expr);

  APPEND_BEGIN(x);

  APPEND(x, ll_listn(2, ll_s(setter), op));

  while ( ! ll_nullQ(args) ) {
    APPEND(x, ll_car(args));
    args = ll_cdr(args);
  }

  APPEND(x, ll_ARG_1);

  ll_return(x);
}
ll_define_macro_end


/*********************************************************/
/* Oaklisp locatives
 * (make-locative <expr>)
 */


ll_define_macro(symbol, make_locative, _1(sym))
{
  /* (make-locative <sym>) => (%make-locative (quote <sym>)) */
  ll_return(ll_listn(2, ll_s(_make_locative), ll_quote(ll_ARG_0)));
}
ll_define_macro_end


ll_define_macro(pair, make_locative, _1(expr))
{
  /* (make-locative (contents <x>)) => <x> */
  if ( ll_EQ(ll_THIS->_car, ll_s(contents)) ) {
    ll_return(ll_car(ll_THIS->_cdr));
  } else {
    /* (make-locative (<op> . <args>))
       => ((locater <op>) . <args>)
    */

    /* Macro-expand (<op> . <args>) first to handle things like
    ** (make-locative (fluid x))
    => ((locater %fluid) 'x)
    */
    ll_v expr = ll_call(ll_o(macro_expand), _1(ll_SELF));
    ll_v op = ll_car(expr);
    ll_v args = ll_cdr(expr);
    
    APPEND_BEGIN(x);
    
    APPEND(x, ll_listn(2, ll_s(locater), op));
    
    while ( ! ll_nullQ(args) ) {
      APPEND(x, ll_car(args));
      args = ll_cdr(args);
    }
    
    ll_return(x);
  }
}
ll_define_macro_end


ll_define_macro(pair, contents, _1(expr))
{
  /* (contents (make-locative <x>)) => <x> */
  if ( ll_EQ(ll_THIS->_car, ll_s(make_locative)) ) {
    ll_return(ll_car(ll_THIS->_cdr));
  } else {
    ll_return(ll_listn(2, ll_s(contents), ll_call(ll_o(macro_expand), _1(ll_SELF))));
  }
}
ll_define_macro_end


ll_define_macro(pair, set_contentsE, __1(expr, value))
{
  ll_v rest = ll_listv(ll_ARGC - 1, ll_ARGV + 1);

  /* (set-contents! (make-locative <x>) <v>) => (set! <x> <v>)*/
  if ( ll_EQ(ll_THIS->_car, ll_s(make_locative)) ) {
    ll_return(ll_cons(ll_s(setE), ll_cons(ll_car(ll_THIS->_cdr), rest)));
  } else {
    ll_return(ll_cons(ll_s(set_contentsE), ll_call(ll_o(macro_expand_list), _1(ll_cons(ll_SELF, rest)))));
  }
}
ll_define_macro_end


/*********************************************************/
/* Macro definition macros */


ll_define_macro(symbol, define_macro, _2(sym, value))
{
  /* (define-macro <sym> <value>) => (%define-macro (quote <sym>) <value>) */
  ll_return(ll_listn(3, ll_s(_define_macro), ll_quote(ll_ARG_0), ll_ARG_1));
}
ll_define_macro_end


ll_define_macro(symbol, macro, _1(sym))
{
  /* (macro <sym>) => (%macro (quote <sym>)) */
  ll_return(ll_listn(2, ll_s(_macro), ll_quote(ll_ARG_0)));
}
ll_define_macro_end


ll_define_macro(pair, define_macro, __1(formals, body))
{
  /* (define-macro (<sym> . <formals>) . <body>) => (define-macro <sym> (lambda <formals> . <body>) */
  ll_v sym = ll_car(ll_ARG_0);
  ll_v formals = ll_cdr(ll_ARG_0);
  ll_v body = ll_listv(ll_ARGC - 1, ll_ARGV + 1);
  ll_return(ll_listn(3,
		     ll_s(define_macro), 
		     sym, 
		       ll_cons(ll_s(lambda),
		       ll_cons(formals,
		               body))));
}
ll_define_macro_end


/*********************************************************/
/* Oaklisp
 * (add-method (<op> (<type> . <slots>) . <formals>) . <body>)
 */


ll_define_macro(pair, add_method, __1(formals, body))
{
  /* (add-method (<op> (<type> . <slots>) . <formals>) . <body>)
       => (%add-method <type> <op> (%method (<formals> . <slots>) . <body>))
   */
  ll_v op = ll_car(ll_ARG_0);
  ll_v x;
  ll_v ts = ll_car(x = ll_cdr(ll_ARG_0));
  ll_v type = ll_car(ts);
  ll_v slots = ll_cdr(ts);
  ll_v formals = ll_cdr(x);
  ll_v body = ll_listv(ll_ARGC - 1, ll_ARGV + 1);
  ll_return(ll_listn(4,
		     ll_s(_add_method),
		     type,
		     op,
		     ll_cons(ll_s(_method),
			     ll_cons(ll_cons(formals, slots),
				     body))));
}
ll_define_macro_end


/*********************************************************/
/* Quickies for number operations */


ll_define_macro(object, ADD, __0(x))
{
  /* (+) => 0 */
  /* (+ <x>) => x */
  /* (+ <x> <y>) => (%+ <x> <y>) */
  /* (+ <x> <y> <z>) => (%+ <x> (%+ <y> <z>)) */
  if ( ll_ARGC == 0 ) {
    ll_return(ll_make_fixnum(0));
  } else {
    ll_v x;
    int i = ll_ARGC;
    
    x = ll_ARGV[-- i];
    while ( i ) {
      x = ll_listn(3, ll_s(_ADD), ll_ARGV[-- i], x);
    }

    ll_return(x);
  }
}
ll_define_macro_end


ll_define_macro(object, SUB, __1(z, y))
{
  /* (- <x>) => (%neg x) */
  /* (- <x> . <y>) = (%- <x> (+ . <y>)) */
  if ( ll_ARGC == 1 ) {
    ll_return(ll_listn(2, ll_s(_NEG), ll_ARG_0));
  } else {
    ll_return(ll_listn(3, ll_s(_SUB), ll_ARG_0, ll_cons(ll_s(ADD), ll_listv(ll_ARGC - 1, ll_ARGV + 1))));
  }
}
ll_define_macro_end


ll_define_macro(object, MUL, __0(x))
{
  /* (*) => 1 */
  /* (* <x>) => x */
  /* (* <x> <y>) = (%* <x> <y>) */
  if ( ll_ARGC == 0 ) {
    ll_return(ll_make_fixnum(1));
  } else {
    ll_v x;
    int i = ll_ARGC;
    
    x = ll_ARGV[-- i];
    while ( i ) {
      x = ll_listn(3, ll_s(_MUL), ll_ARGV[-- i], x);
    }

    ll_return(x);
  }
}
ll_define_macro_end


ll_define_macro(object, DIV, __1(z, y))
{
  /* (/ <x>) => (%/ 1 x) */
  /* (/ <x> . <y>) = (%/ <x> (* . <y>)) */
  if ( ll_ARGC == 1 ) {
    ll_return(ll_listn(3, ll_s(_DIV), ll_make_fixnum(1), ll_ARG_0));
  } else {
    ll_return(ll_listn(3, ll_s(_DIV), ll_ARG_0, ll_cons(ll_s(MUL), ll_listv(ll_ARGC - 1, ll_ARGV + 1))));
  }
}
ll_define_macro_end


/*********************************************************/

#undef APPEND_BEGIN
#undef APPEND
#undef INSERT

