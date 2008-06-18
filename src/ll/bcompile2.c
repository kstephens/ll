#ifndef __rcs_id__
#ifndef __rcs_id_ll_bcompile2_c__
#define __rcs_id_ll_bcompile2_c__
static const char __rcs_id_ll_bcompile2_c[] = "$Id: bcompile2.c,v 1.3 2008/05/26 07:57:42 stephens Exp $";
#endif
#endif /* __rcs_id__ */


/***************************************************************************/

/*
** 2nd pass:
** 1. fold constants.
** 2. emit code.
*/


/**********************************************************************/

#include "bcompile.h"

#if 0
#define DEBUG_PRINTF fprintf
#else
#define DEBUG_PRINTF (void)
#endif

/**********************************************************************/

#define SELF ll_SELF
#define IR ll_ARG_1

/**********************************************************************/

ll_define_primitive(object, _ir_compile2_debug_expr, _2(self, ir), _0())
{
  ll_assert_ref(IR);
  /* NOTHING */
}
ll_define_primitive_end


ll_define_primitive(pair, _ir_compile2_debug_expr, _2(self, ir), _0())
{
  ll_assert_ref(IR);
  ll_call(ll_o(_ir_emit_constant), _2(IR, ll_SELF));
  ll_call(ll_o(_ir_emit), _2(IR, ll_s(_bcCdebug_expr)));
  ll_call_tail(ll_o(_ir_emit), _2(IR, ll_s(pop)));
}
ll_define_primitive_end


ll_define_primitive(_ir, _ir_compile2_debug_expr, _2(self, ir), _0())
{
  ll_v expr;

  ll_assert_ref(IR);

  expr = ll_cons(ll_s(closure),
		 ll_cons(ll_THIS->_formals,
			 ll_THIS->_body));

  ll_call(ll_o(_ir_emit_constant), _2(IR, expr));
  ll_call(ll_o(_ir_emit), _2(IR, ll_s(_bcCdebug_expr)));
  ll_call_tail(ll_o(_ir_emit), _2(IR, ll_s(pop)));
}
ll_define_primitive_end


/**********************************************************************/


#define CAR_POSQ ll_THIS_ISA(_ir, IR)->_car_posQ
#define TAIL_POSQ ll_ARG_2


ll_define_primitive(constant, _ir_compile2, _3(self, ir, tail_posQ), _0())
{
  ll_assert_ref(IR);
  ll_call(ll_o(_ir_emit_constant), _2(IR, SELF));
  if ( ll_unbox_boolean(TAIL_POSQ) ) {
    ll_call_tail(ll_o(_ir_emit), _2(IR, ll_s(rtn)));
  }
}
ll_define_primitive_end


/*
** Compile into code to generate a value for an exports vector.
*/
ll_define_primitive(pair, _ir_compile2_export, _2(self, ir), _0())
{
  ll_v x = ll_SELF;
  ll_v var = ll_car(x);
  ll_v type = var_type(x);
  ll_v index = var_index(x);

  ll_assert_ref(IR);

  /* slot ref */
  if ( ll_EQ(type, ll_s(slot)) ) {
    /* Create a const containing the global var name */
    index = ll_call(ll_o(_ir_const_index), _2(IR, var));
  } else
  if ( ll_EQ(type, ll_s(glo)) ) {
    ll_abort();
  }

  /* undef export is a local from a car-pos sub lambda */
  if ( ll_EQ(type, ll_undef) ) {
    ll_call_tail(ll_o(_ir_compile2), _3(ll_undef, IR, ll_f));
  } else {
    ll_v emit_type = type;

    /* env_loc is an environment slot containing a locative */
    if ( ll_EQ(type, ll_s(env_loc)) ) {
      emit_type = ll_s(env);
    }

    if ( ll_unbox_boolean(index) ) {
      ll_call(ll_o(_ir_emit_with_int), _3(IR, emit_type, index));
    } else {
      ll_call(ll_o(_ir_emit), _2(IR, emit_type));
    }

    /*
    ** arg and local values are stored directly into the exports vector.
    ** so put the value, not a locative to the arg or local in the exports vector.
    */
    if ( ll_EQ(type, ll_s(arg)) 
	 || ll_EQ(type, ll_s(local)) 
	 || ll_EQ(type, ll_s(env_loc))
      ) {
      ll_call_tail(ll_o(_ir_emit), _2(IR, ll_s(contents)));
    }
  }
}
ll_define_primitive_end


void _ll_gdb_stop_at()
{
}


static int in_preamble = 0;

/*
** Compiles a symbol into an instruction that generates a get,
** locative, or set operation.
** to the variable.
*/
ll_define_primitive(symbol, _ir_compile2_gen, _4(self, ir, tail_posQ, mode), _0())
{
  ll_v x = ll_call(ll_o(_ir_var_scope), _2(IR, SELF));
  ll_v mode = ll_ARG_3;
  ll_v var = SELF;
  ll_v type = var_type(x);
  ll_v index = var_index(x);
  ll_v co = var_closed_overQ(x);
  ll_v isn = ll_nil;

  _ll_gdb_stop_at();

  if ( ll_DEBUG(compile_emit) ) {
    ll_format(ll_undef, "  ~O: compile2-gen: ~S ~S\n", 3, IR, mode, x);
  }

  ll_assert_ref(IR);

  /* If it's closed over and environment has been exported.
   *
   * The environment has not been exported until 
   * the rest args creation
   * and other preambles have been emited.
   */
  if ( ll_unbox_boolean(co) 
       && ! in_preamble 
       /* && ll_unbox_boolean(ll_THIS_ISA(_ir, IR)->_env_exportedQ) */
       ) {
    /* Get the locative to the exports[slots] value */
    ll_call(ll_o(_ir_emit_with_int), _3(IR, ll_s(export), co));

    /*
    ** If the closed-over variable is an argument or a local
    ** the value is stored directly in the exported environment
    ** vector.
    **
    ** Otherwise the environment slot contains a locative
    ** to the actual value, probably an object slot.
    */
    if ( ! (ll_EQ(type, ll_s(arg)) || ll_EQ(type, ll_s(local))) ) {
      ll_call(ll_o(_ir_emit), _2(IR, ll_s(contents)));
    }
  }

  /* slot ref */
  else if ( ll_EQ(type, ll_s(slot)) ) {
    /* Create a const containing the slot name. */
    index = ll_call(ll_o(_ir_const_index), _2(IR, var));

    /* Emit code to create locative to slot. */
    ll_call(ll_o(_ir_emit_with_int), _3(IR, type, index));
  }

  /* glo ref */
  else if ( ll_EQ(type, ll_s(glo)) ) {

    if ( ll_EQ(mode, ll_s(get)) ) {
      isn = ll_s(glo);
    }
    else if ( ll_EQ(mode, ll_s(set)) ) {
      isn = ll_s(glo_set);
    }
    else if ( ll_EQ(mode, ll_s(loc)) ) {
      isn = ll_s(glo_loc);
    } 
    else {
      ll_format(ll_undef, "bad mode ~S\n", 1, mode);
      ll_abort();
    }

    ll_call(ll_o(_ir_emit_with_int), _3(IR, isn, index));
    mode = ll_nil;
  } 
  else if ( ll_EQ(type, ll_s(env_loc)) ) {
    /* Since (env i) => &ENV[i] is a locative
       dereference it to get the locative
       stored in the environment vector.
    */
    if ( ll_unbox_boolean(index) ) {
      ll_call(ll_o(_ir_emit_with_int), _3(IR, ll_s(env), index));
    } else {
      ll_call(ll_o(_ir_emit), _2(IR, ll_s(env)));
    }
    ll_call(ll_o(_ir_emit), _2(IR, ll_s(contents)));
  }
  else {
    /* * ref */
    if ( ll_unbox_boolean(index) ) {
      ll_call(ll_o(_ir_emit_with_int), _3(IR, type, index));
    } else {
      ll_call(ll_o(_ir_emit), _2(IR, type));
    }
  }

  if ( ll_EQ(mode, ll_s(get)) ) {
    ll_call(ll_o(_ir_emit), _2(IR, ll_s(contents)));
  }
  else if ( ll_EQ(mode, ll_s(set)) ) {
    ll_call(ll_o(_ir_emit), _2(IR, ll_s(set_contents)));
  }

  if ( ll_unbox_boolean(TAIL_POSQ) ) {
    ll_call_tail(ll_o(_ir_emit), _2(IR, ll_s(rtn)));
  }
}
ll_define_primitive_end


ll_define_primitive(symbol, _ir_compile2_get, _3(self, ir, tail_posQ), _0())
{
  ll_call_tail(ll_o(_ir_compile2_gen), _4(SELF, IR, TAIL_POSQ, ll_s(get)));
}
ll_define_primitive_end


ll_define_primitive(symbol, _ir_compile2_setE, _3(self, ir, tail_posQ), _0())
{
  ll_v x = ll_call(ll_o(_ir_var_scope), _2(IR, SELF));
  ll_v type = var_type(x);

  ll_assert_ref(IR);

  /* glo ref */
  if ( ll_EQ(type, ll_s(glo)) ) {
    ll_v index = var_index(x);
    ll_call(ll_o(_ir_emit_with_int), _3(IR, ll_s(glo_set), index));
  } else {
    /*
    ** 1. Compile code to create a locative to <self> on the stack,
    ** 2. Compile code to set the value of the locative on the stack.
    */
    ll_call(ll_o(_ir_compile2_loc), _3(SELF, IR, ll_f));
    ll_call(ll_o(_ir_emit), _2(IR, ll_s(set_contents)));
  }

  /*
  ** If we are a tail-pos (set! ...) expression,
  ** emit code to return an unspecified value.
  */
  if ( ll_unbox_boolean(TAIL_POSQ) ) {
    ll_call_tail(ll_o(_ir_emit), _2(IR, ll_s(rtn)));
  }
}
ll_define_primitive_end


/*
** Compiles a symbol into an instruction that generates a locative.
** to the variable.
*/
ll_define_primitive(symbol, _ir_compile2_loc, _3(self, ir, tail_posQ), _0())
{
  ll_call_tail(ll_o(_ir_compile2_gen), _4(SELF, IR, TAIL_POSQ, ll_s(loc)));
}
ll_define_primitive_end


ll_define_primitive(symbol, _ir_compile2, _3(self, ir, tail_posQ), _0())
{
  ll_call(ll_o(_ir_compile2_get), _3(SELF, IR, TAIL_POSQ));

  if ( ll_unbox_boolean(TAIL_POSQ) ) {
    ll_call(ll_o(_ir_emit), _2(IR, ll_s(rtn)));
  }
}
ll_define_primitive_end



ll_define_primitive(list, _ir_compile2_body, _3(body, ir, tail_posQ), _0())
{
  ll_v x = ll_SELF;

  ll_assert_ref(IR);

  DEBUG_PRINTF(stderr, "(");

  if ( ll_nullQ(x) ) {
    /* (lambda <formals>) => (lambda <formals> (if #f #f)) */
    ll_call_tail(ll_o(_ir_compile2), _3(ll_unspec, IR, TAIL_POSQ));
  } else {
    /* Compile the first body sexprs as non tail-pos */ 
    ll_v x_cdr, e;
    int found_docstring = 0;

    x_cdr = ll_cdr(x);
    while ( ! ll_nullQ(x_cdr) ) {
      e = ll_car(x);

      if ( 0 ) {
	ll_format(ll_undef, "~O: compile2-body: ~S\n", 2, IR, e);
      }

      /* The first non-tail body string is documentation. */
      if ( ! found_docstring && ll_unbox_boolean(ll_call(ll_o(stringQ), _1(e))) ) {
	found_docstring ++;
	ll_call(ll_o(_ir_set_propE), _3(IR, ll_s(doc), e));
      } else if ( ll_unbox_boolean(ll_call(ll_o(constantQ), _1(e))) ) {
	/* A non-tail body constant is ignored. */
      } else {
#ifndef DEBUG_EXPR
#define DEBUG_EXPR 0
#endif
	if ( DEBUG_EXPR ) {
	  ll_call(ll_o(_ir_compile2_debug_expr), _2(e, IR));
	}

	/* Must be a regular expression. */
	ll_call(ll_o(_ir_compile2), _3(e, IR, ll_f));
	
	/* Discard each non tail-pos result. */
	ll_call(ll_o(_ir_emit), _2(IR, ll_s(pop)));
      }

      x = x_cdr;
      x_cdr = ll_cdr(x_cdr);
    }
      
    /* Compile the last body sexpr as tail-pos */
    e = ll_car(x);
    if ( DEBUG_EXPR ) {
      ll_call(ll_o(_ir_compile2_debug_expr), _2(e, IR));
    }

    ll_call_tail(ll_o(_ir_compile2), _3(e, IR, TAIL_POSQ));
  }

  DEBUG_PRINTF(stderr, ")");
}
ll_define_primitive_end


ll_define_primitive(_ir, _ir_compile2_body, _3(self, ir, tail_posQ), _0())
{
  /*
  ** If we are not car-pos lambda
  ** our tail body expr is always tail-pos.
  ** E.g.:
  (lambda (foo) (outer-lambda)
     (not-tail-pos)
     (lambda (bar) (this-lambda)
       (not-tail-pos)
       (tail-pos)
     ))
  ** If we are car-pos,
  ** our tail body expr is tail-pos only if we are tail-pos.
  ** E.g.:
  (lambda (foo) (outer-lambda)
     (not-tail-pos)
     ((lambda (bar)
       (not-tail-pos)
       (tail-pos)
       ) 'bar-value)
     )
  */
  ll_assert_ref(IR);

  DEBUG_PRINTF(stderr, "[");

  in_preamble = 0;

  if ( ! ll_unbox_boolean(ll_THIS->_car_posQ) ) {
    TAIL_POSQ = ll_t;

    {
      ll_v car;

      if ( ! ll_nullQ(ll_THIS->_body) && ll_pairQ(car = ll_car(ll_THIS->_body)) && ((car = ll_car(car)), ll_EQ(car, ll_s(_bcCdebug)) || ll_EQ(car, ll_s(_bcCdebug_off))) ) {
	/* (%bc:debug <x>) */
	ll_call(ll_o(_ir_emit), _2(IR, car));
	ll_call(ll_o(_ir_emit), _2(IR, ll_s(pop)));
      }
    }
      
    /* Emit stack probe code. */
    ll_THIS->_probe_pos = ll_call(ll_o(_ir_emit_with_int), _3(ll_SELF, ll_s(probe), ll_BOX_fixnum(0)));

    /* Emit local save code */
    if ( ll_unbox_fixnum(ll_THIS->_max_n_locals) > 0 ) {
      ll_call(ll_o(_ir_emit_with_int), _3(ll_SELF, ll_s(locals), ll_THIS->_max_n_locals));
    }

    if ( ll_unbox_boolean(ll_THIS->_rest_argQ) ) {
      /* Emit rest-arg code */
      ll_v var;

      var = ll_assq(ll_THIS->_rest_argQ, ll_THIS->_scope);

      in_preamble = 1;

      if ( ll_unbox_boolean(var_referencedQ(var)) ) {

	/* Code to check no of args and push a list of args on the stack */
	ll_call(ll_o(_ir_emit_with_int), _3(ll_SELF, ll_s(nargs_rest), ll_THIS->_argc));

	/* Code to set the rest-arg cell. */
	/* Note: see _env_exportedQ. */
	ll_call(ll_o(_ir_compile2_setE), _3(ll_THIS->_rest_argQ, ll_SELF, ll_f));

	/* Pop set! result. */
	ll_call(ll_o(_ir_emit), _2(ll_SELF, ll_s(pop)));
      } else {
	/* Code to check no of args */
	ll_call(ll_o(_ir_emit_with_int), _3(ll_SELF, ll_s(nargs_rest_), ll_THIS->_argc));
      }

    } else {
      /* Emit arg check */

      ll_call(ll_o(_ir_emit_with_int), _3(ll_SELF, ll_s(nargs), ll_THIS->_argc));
    }

    /* IS THIS APPLICABLE IN CARPOS? */
    /* Emit exported environment code */
    if ( ! ll_nullQ(ll_THIS->_env) ) {
      /* The environment list is in reverse environment index order. */
      /* We push the environment values on the stack in order. */
      ll_LIST_LOOP(ll_THIS->_env, x);
      {
	if ( ll_DEBUG(compile_emit) ) {
	  ll_format(ll_undef, "  ~O create-env ~S\n", 2, ll_SELF, x);
	}
	ll_call(ll_o(_ir_compile2_export), _2(x, ll_SELF));
	if ( ll_DEBUG(compile_emit) ) {
	  ll_format(ll_undef, "  ~O create-env ~S : DONE\n", 2, ll_SELF, x);
	}
      }
      ll_LIST_LOOP_END;

      ll_call(ll_o(_ir_emit_with_int), _3(ll_SELF, ll_s(make_exports), ll_THIS->_env_length));
    }

  }

  /* Mark environment as exported. */
  in_preamble = 0;
  ll_THIS->_env_exportedQ = ll_t;

  /* Compile our body exprs */
  ll_call(ll_o(_ir_compile2_body), _3(ll_THIS->_body, IR, TAIL_POSQ));

  /* Backpatch stack probe code */
  if ( ! ll_unbox_boolean(ll_THIS->_car_posQ) ) {
    int max_stack_depth;

    ll_v code = ll_call(ll_o(_ir_code), _1(ll_SELF));
    max_stack_depth = _ll_bc_max_stack_depth((ll_bc_t*) ll_THIS_ISA(string, code)->_array);
    /* fprintf(stderr, "ll: bc: max_stack_depth %d\n", max_stack_depth); */
    max_stack_depth += 2;
    ll_call(ll_o(_ir_emit_patch), _3(ll_SELF, ll_THIS->_probe_pos, ll_make_fixnum(max_stack_depth)));
  }

  DEBUG_PRINTF(stderr, "]");

}
ll_define_primitive_end


ll_define_primitive(_ir, _ir_compile2, _3(self, ir, tail_posQ), _0())
{
  ll_assert_ref(IR);

  /* self is the child ir, ir is the parent ir. */
  /* Child's body is compiled in its own context. */
  /* Code to generate child is compile in parent context. */

  /* Compile the child's body */
  ll_call(ll_o(_ir_compile2_body), _3(ll_SELF, ll_SELF, TAIL_POSQ));

  /*
  ** If the child is not car-pos, create code to make the child's method.
  ** If the child is car-pos, it's code has been inlined in parent method.
  */
  if ( ! ll_unbox_boolean(ll_THIS->_car_posQ) ) {
    /* Store the compiled code as a const */
    ll_v new_code = ll_call(ll_o(_ir_code), _1(ll_SELF));
    ll_v make_method = ll_s(make_method);
     
    /* Emit code in parent to create the method */

    /* Does this method need an environment? ask the parent to give to us */
    if ( ! ll_nullQ(ll_THIS->_imports) ) {
      ll_call(ll_o(_ir_emit), _2(IR, ll_s(exports)));
      make_method = ll_s(make_method_env);
    }

    /* Push the child's method properties on the stack. */
    ll_call(ll_o(_ir_emit_constant), 
	    _2(IR,
	       ll_call(ll_o(_ir_method_props), _1(ll_SELF))
	       )
	    );

    /* Push the child's code vector on the stack */
    ll_call(ll_o(_ir_emit_constant), _2(IR, new_code));

    /* Emit code in the parent to create the child's method */
    ll_call(ll_o(_ir_emit), _2(IR, make_method));
  }
}
ll_define_primitive_end


/*
car-pos lambda example:

((lambda (a b) (list a b a b)) (+ 1 2) 5)
=> (3 5 3 5)
*/
ll_define_primitive(pair, _ir_compile2, _3(self, ir, tail_posQ), _0())
{
  ll_v car = ll_THIS->_car;
  ll_v x = ll_THIS->_cdr;
  int length = ll_unbox_fixnum(ll_call(ll_o(length), _1(ll_SELF)));
  int tail_posQ = ll_unbox_boolean(TAIL_POSQ);

  ll_assert_ref(IR);

  if ( _ll_bcc_2 ) {
    ll_format(ll_undef, "\n\ncompile-2:\n  ", 0);
    ll_call(ll_o(_write_ir), _2(ll_ARG_1, ll_undef));
  }
  if ( ll_EQ(car, ll_s(_bcc_2)) ) {
    ll_format(ll_undef, "\n\ncompile-2:\n  ", 0);
    ll_call(ll_o(_write_ir), _2(ll_ARG_1, ll_undef));
    ll_return(ll_SELF);
  }

  DEBUG_PRINTF(stderr, "*");

  /* ll_format(ll_undef, "%ir-compile2 ~S\n", 1, ll_SELF); */

  if ( ll_EQ(car, ll_s(quote)) ) {
    /* (quote <x>) */

  emit_constant:
    ll_call(ll_o(_ir_emit_constant), _2(IR, ll_car(x)));
  } else if ( ll_EQ(car, ll_s(_lambda)) ) {
    /* (%lambda #<%ir>) */

    ll_v expr = ll_car(x);

    /* Compile the method expresion. */
    ll_call(ll_o(_ir_compile2), _3(expr, IR, ll_f));

    /* Construct a lambda <operation> from the <method>. */
    ll_call(ll_o(_ir_emit), _2(IR, ll_s(lambda)));
   } else if ( ll_EQ(car, ll_s(_make_locative)) ) {
    /* (%make-locative (quote <var>)) */

    ll_v var = ll_car(ll_cdr(ll_car(x)));

    /* Compile the locative expression */
    ll_call_tail(ll_o(_ir_compile2_loc), _3(var, IR, TAIL_POSQ));
  } else if ( ll_EQ(car, ll_s(setE)) ) {
    /* (set! <var> <val>) */

    ll_v var = ll_car(x);
    ll_v value = ll_car(ll_cdr(x));
    /* Compile the value expression */
    ll_call(ll_o(_ir_compile2), _3(value, IR, ll_f));

    /* Compile the locative expression */
    ll_call_tail(ll_o(_ir_compile2_setE), _3(var, IR, TAIL_POSQ));
  } else if ( ll_EQ(car, ll_s(define)) ) {
    /* (define <var> <val> ...) */

    ll_v var = ll_car(x);
    ll_v values = ll_cdr(x);
    ll_v value = ll_car(values);
    ll_v b = ll_call(ll_o(_ir_var_scope), _2(IR, var));
   
    DEBUG_PRINTF(stderr, "\n 2 %s ", (char*) ll_po(var));

    /* If <var> is global, rewrite as:
    **   (%define '<var> <val> ...)
    */
    if ( ll_EQ(var_type(b), ll_s(glo)) ) {
      x = ll_cons(ll_s(_define), ll_cons(ll_quote(var), values));
      ll_call_tail(ll_o(_ir_compile2), _3(x, IR, TAIL_POSQ));
    } else {
      /* Compile the value expression */
      ll_call(ll_o(_ir_compile2), _3(value, IR, ll_f));

      /* Compile the locative expression */
      ll_call_tail(ll_o(_ir_compile2_setE), _3(var, IR, TAIL_POSQ));
    }
  } else if ( ll_EQ(car, ll_s(if)) ) {
    /* (if <pred> <true-branch> [<false-branch>]) */

    ll_v pred, true_expr, false_expr;

    pred = ll_car(x); x = ll_cdr(x);
    true_expr = ll_car(x); x = ll_cdr(x);
    false_expr = ll_nullQ(x) ? ll_unspec : ll_car(x);

    if ( ll_unbox_boolean(ll_call(ll_o(_ir_constantQ), _2(pred, IR))) ) {
      pred = ll_call(ll_o(eval), _1(pred));

      if ( ll_unbox_boolean(pred) ) {
	/* Compile true branch */
	ll_call(ll_o(_ir_compile2), _3(true_expr, IR, TAIL_POSQ));
      } else {
	/* Compile false branch */
	ll_call(ll_o(_ir_compile2), _3(false_expr, IR, TAIL_POSQ));
      }
    } else {
      ll_v ifpos, jmppos, falsepos, endpos;

      /* Compile predicate */
      ll_call(ll_o(_ir_compile2), _3(pred, IR, ll_f));
      
      /* Emit jf ins */
      ifpos = ll_call(ll_o(_ir_emit_with_int), _3(IR, ll_s(jf), ll_make_fixnum(0)));
      
      /* Compile true branch */
      ll_call(ll_o(_ir_compile2), _3(true_expr, IR, TAIL_POSQ));
      
      if ( ! tail_posQ ) {
	/* Emit jmp ins to after false branch */
	/* Don't bother if tail_pos because true branch will return immediately. */
	jmppos = ll_call(ll_o(_ir_emit_with_int), _3(IR, ll_s(jmp), ll_make_fixnum(0)));
      }

      /* false branch begins here. */
      falsepos = ll_call(ll_o(_ir_emit_pos), _1(IR));
      
      /* Compile false branch */
      ll_call(ll_o(_ir_compile2), _3(false_expr, IR, TAIL_POSQ));
      
      /* Find the end of the false branch */
      endpos = ll_call(ll_o(_ir_emit_pos), _1(IR));
      
      /* Patch jf, jmp */
      ll_call(ll_o(_ir_emit_patch_relative), _3(IR, ifpos, falsepos));

      if ( ! tail_posQ ) {
	/* Don't bother patching, jmppos because it wasn't emited. */
	ll_call(ll_o(_ir_emit_patch_relative), _3(IR, jmppos, endpos));
      }
    }

    ll_return(ll_SELF);
  } else if ( ll_EQ(car, ll_s(_method)) ) {
    /* (%method (<formals> . <slots>) . <body>) */
    ll_return(IR);
  } else {
    /* (<op> . <args>) */

    /* Is op a car-pos lambda? */
    if ( ll_unbox_boolean(ll_call(ll_o(_irQ), _1(car))) ) {
      ll_v car_ir = car;
      ll_v formals = ll_call(ll_o(_ir_formals), _1(car_ir));
      ll_v args = x;
      ll_v body = ll_call(ll_o(_ir_body), _1(car_ir));

      if ( 0 ) {
	ll_format(ll_undef, "%ir: car-pos-lambda\n\tformals ~S\n\t", 1, ll_SELF);
      }
      
      /* <op> is a car-pos lambda */
      /* ((lambda <formals> . <body>) . <args>) */
      
      /* Do args in reverse order, storing results in locals */
      ll_call(ll_o(_ir_compile2_carpos_args), _4(formals, IR, args, car_ir));

      /* Compile the car_ir's body inline, in the car_ir's context */
      ll_call(ll_o(_ir_compile2_body), _3(body, car_ir, TAIL_POSQ));
    } else {
      ll_v call;

      if ( ll_EQ(car, ll_s(super)) ) {
	/* (super <super> <op> . <args>) */
	call = tail_posQ ? ll_s(call_super_tail) : ll_s(call_super);
	/* The stack will be <super> <op> . <args> */
	/* We will pop  <super> and <op> then call _ll_call_superv(<op>,<super>,ARGC).  Therefore the <super> value is not really a part of the arg list. */
	/*
	  length --;
	  ll_assert(ir,length >= 0);
	*/
	x = ll_THIS->_cdr;
      } else {

	call = tail_posQ ? ll_s(call_tail) : ll_s(call);
	x = ll_SELF;
      }

      /* Emit code for special ops. */
      if ( ll_EQ(car, ll_s(_bcCdebug)) ||
	    ll_EQ(car, ll_s(_bcCdebug_off)) ||
	    ll_EQ(car, ll_s(_bcCdebug_expr))
	   ) {
	ll_call(ll_o(_ir_emit), _2(IR, car));
      } else {
	
	/* Emit code for <op> . <args> in reverse order, storing results on stack */
	ll_call(ll_o(_ir_compile2_args), _4(x, IR, ll_f, ll_f));
      

	/* Emit call or tail-call */
	ll_call(ll_o(_ir_emit_with_int), _3(IR, call, ll_make_fixnum(length - 1)));
      }
    }
  }

  if ( tail_posQ ) {
    ll_call_tail(ll_o(_ir_emit), _2(IR, ll_s(rtn)));
  }

  ll_return(ll_SELF);
}
ll_define_primitive_end


/* Emit code for argument list in reverse order */
ll_define_primitive(null, _ir_compile2_args, _4(self, ir, tail_posQ, pop), _1(no_side_effect,"#t"))
{
  ll_assert_ref(IR);
}
ll_define_primitive_end


ll_define_primitive(pair, _ir_compile2_args, _4(self, ir, tail_posQ, pop), _0())
{
  size_t l = _ll_list_length(ll_SELF);

  ll_assert_ref(IR);

  /* Emit val stack buffer probe for argument list. */
  ll_call(ll_o(_ir_emit_with_int), 
	  _3(ll_ARG_1, 
	     ll_s(probe),
	     ll_BOX_fixnum(l)));

  ll_call_tail(ll_o(_ir_compile2_args_no_probe), _4(ll_SELF, ll_ARG_1, ll_ARG_2, ll_ARG_3));
}
ll_define_primitive_end


ll_define_primitive(pair, _ir_compile2_args_no_probe, _4(self, ir, tail_posQ, pop), _0())
{
  ll_assert_ref(IR);
  ll_call(ll_o(_ir_compile2_args_no_probe), _4(ll_THIS->_cdr, ll_ARG_1, ll_ARG_2, ll_ARG_3));
  ll_call(ll_o(_ir_compile2), _3(ll_THIS->_car, ll_ARG_1, ll_ARG_2));
  if ( ll_unbox_boolean(ll_ARG_3) ) {
    ll_call_tail(ll_o(_ir_emit), _2(IR, ll_s(pop)));
  }
}
ll_define_primitive_end


ll_define_primitive(null, _ir_compile2_args_no_probe, _4(self, ir, tail_posQ, pop), _1(no_side_effect,"#t"))
{
  ll_assert_ref(IR);
}
ll_define_primitive_end


/****************************/


#define FORMALS ll_ARG_0
#define ARGS ll_ARG_2
#define CAR_IR ll_ARG_3


ll_define_primitive(null,_ir_compile2_carpos_args, _4(formals,ir,args,car_ir), _1(no_side_effect,"#t"))
{
  ll_assert_ref(IR);
  if ( ! ll_nullQ(ARGS) ) {
    ll_return(_ll_error(ll_ee(argument), 1, ll_s(nargs), ll_undef));
  }
}
ll_define_primitive_end


ll_define_primitive(symbol,_ir_compile2_carpos_args, _4(formals,ir,args,car_ir), _0())
{
  /* formals is the name of the rest arg */
  ll_v varname, var;

  ll_assert_ref(IR);

  varname = FORMALS;
  var = ll_assq(varname, ll_call(ll_o(_ir_scope), _1(CAR_IR)));

  if ( ll_unbox_boolean(var_referencedQ(var)) ) {
    /* Compile the argument expressions in reverse order */
    ll_call(ll_o(_ir_compile2_args), _4(ARGS, IR, ll_f, ll_f));
  
    /* Emit isn to create a list from args */
    ll_call(ll_o(_ir_emit_with_int), _3(IR, ll_s(list), ll_call(ll_o(length), _1(ARGS))));

    /* Emit isn to set to the rest arg */
    ll_call_tail(ll_o(_ir_compile2_setE), _3(CAR_IR, varname, ll_f));
  } else {
    /* Compile the argument expressions in reverse order, popping the results */
    ll_call_tail(ll_o(_ir_compile2_args), _4(ARGS, IR, ll_f, ll_t));
  }

}
ll_define_primitive_end


ll_define_primitive(pair,_ir_compile2_carpos_args, _4(formals,ir,args,car_ir), _0())
{
  ll_assert_ref(IR);

  /* Compile remaining argument expressions in reverse order */
  ll_call(ll_o(_ir_compile2_carpos_args), _4(ll_THIS->_cdr, IR, ll_cdr(ARGS), CAR_IR));

  /* Do this one now */
  {
    ll_v varname, var, value;

    varname = ll_THIS->_car;
    var = ll_assq(varname, ll_call(ll_o(_ir_scope), _1(CAR_IR)));
    value = ll_car(ll_ARG_2);

    /* Compile the argument expressions in reverse order */
    ll_call(ll_o(_ir_compile2), _3(value, IR, ll_f));
    
    if ( ll_unbox_boolean(var_referencedQ(var)) ) {
      /* Emit isn to create a locative to the rest arg */
      ll_call_tail(ll_o(_ir_compile2_setE), _3(varname, CAR_IR, ll_f));
    } else {
      /* Emit pop result */
      ll_call_tail(ll_o(_ir_emit), _2(IR, ll_s(pop)));
    }
  }
}
ll_define_primitive_end


#undef formals
#undef args
#undef car_ir


/**********************************************************************/

/**********************************************************************/


