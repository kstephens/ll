#ifndef __rcs_id__
#ifndef __rcs_id_ll_bcompile_c__
#define __rcs_id_ll_bcompile_c__
static const char __rcs_id_ll_bcompile_c[] = "$Id: bcompile.c,v 1.50 2008/05/26 12:25:32 stephens Exp $";
#endif
#endif /* __rcs_id__ */


/* 
   1st pass:

   1. Translate (%method (<formals> . <slots>) . <body>) into %ir objects.
   2. Determine variable scope.
   3. Allocate locals.

   2nd pass:

   1. fold constants.
   2. emit code.
*/

/***************************************************************************/
/* byte-code-method construction */
/*

The byte-code-method object is created with a byte-code string, a constants vector, a properties list and an optional environment vector.

The byte-code string and properties list are constant and stored in the top-level method's constant vector.

The environment vector is created in the enclosing method.

Nested byte-code-methods share a constants vector with the top-level
byte-code-method.

The first element of the constants vector is a vector of global symbol referenced.

The second element of the constants vector is a vector of global symbol bindings initialized to be all #f.

The 'glo instruction indexes into the global symbol table and attempts to get a binding object to store in the global symbol bindings table.  The 'glo instruction is then rewritten as 'glo_.

The 'glo_ instruction indexes directly into the global symbol bindings table, since the binding has already been resolved by 'glo.

The remaining constant vector elements are all the unique constants (quoted values) referenced by the entire top-level method.

The top-level byte-code constant vector can be written out to a file
for reloading a later date.


Variable scope structure:

(
  name ; the name of the variable
  type : local, arg, slot, exp, env, env-loc, glo
  index : index used to find the vairable locative, see type
  closed-over? : is the vairable referenced in a child scope?
  referenced? : is the var referenced at all?
  rest-arg? : is the var a rest arg?
)

*/

/***************************************************************************/

#include "bcompile.h"

int _ll_bcc_1 = 0;
int _ll_bcc_2 = 0;

#if 0
#define DEBUG_PRINTF fprintf
#else
#define DEBUG_PRINTF (void)
#endif


/***************************************************************************/


ll_define_primitive(_ir, _write_object_deep, _3(obj, port, op), _0())
{
  ll_format(ll_ARG_1, "#<~N ~W ~S ~S>", 4, ll_TYPE(ll_SELF), ll_SELF, ll_THIS->_formals, ll_THIS->_body);
}
ll_define_primitive_end


ll_define_primitive(_ir, _write_ir, _2(obj, port), _0())
{
  ll_format(ll_ARG_1, "'slots ~S 'formals ~S\n", 2, ll_THIS->_slots, ll_THIS->_formals);

  ll_format(ll_ARG_1, "  'max-n-locals ~S 'n-locals ~S 'env-length ~S\n", 
	    3,
	    ll_THIS->_max_n_locals, 
	    ll_THIS->_n_locals,
	    ll_THIS->_env_length);

  ll_format(ll_ARG_1, "  'car-pos? ~S 'rest-arg? ~S\n", 
	    2,
	    ll_THIS->_car_posQ, 
	    ll_THIS->_rest_argQ,
	    ll_undef);

  if ( ! ll_nullQ(ll_THIS->_scope) ) {
    ll_format(ll_ARG_1, "  'scope (\n", 0);
    ll_format(ll_ARG_1, "   ;(name type index closed-over? referenced? rest-arg?)\n", 0);
    ll_LIST_LOOP(ll_THIS->_scope, x);
    {
      ll_format(ll_ARG_1, "    ~S\n", 1, x);
    }
    ll_LIST_LOOP_END;
    ll_format(ll_ARG_1, "  )\n", 0);
  }

  if ( ! ll_nullQ(ll_THIS->_imports) ) {
    ll_format(ll_ARG_1, "  'imports (\n", 0);
    ll_LIST_LOOP(ll_THIS->_imports, x);
    {
      ll_format(ll_ARG_1, "    ~S\n", 1, x);
    }
    ll_LIST_LOOP_END;
    ll_format(ll_ARG_1, "  )\n", 0);
  }

  if ( ! ll_nullQ(ll_THIS->_env) ) {
    ll_format(ll_ARG_1, "  'env (\n", 0);
    ll_LIST_LOOP(ll_THIS->_env, x);
    {
      ll_format(ll_ARG_1, "    ~S\n", 1, x);
    }
    ll_LIST_LOOP_END;
    ll_format(ll_ARG_1, "  )\n", 0);
  }

  if ( ! ll_nullQ(ll_THIS->_body) ) {
    ll_format(ll_ARG_1, "  'body (\n", 0);
    ll_LIST_LOOP(ll_THIS->_body, x);
    {
      ll_format(ll_ARG_1, "    ~S\n", 1, x);
    }
    ll_LIST_LOOP_END;
    ll_format(ll_ARG_1, "  )\n", 0);
  }

  if ( ll_unbox_boolean(ll_THIS->_code_gen) ) {
    ll_format(ll_ARG_1, "  'code_gen (\n", 0);
    ll_VECTOR_LOOP(ll_THIS->_code_gen, x);
    {
      ll_format(ll_ARG_1, "    ~S\n", 1, x);
    }
    ll_VECTOR_LOOP_END;
    ll_format(ll_ARG_1, "  )\n", 0);
  }
}
ll_define_primitive_end


#define IR ll_SELF


ll_define_primitive(_ir, _ir_code_ir, _1(ir), _1(no_side_effect, "#t"))
{
  ll_return(ll_THIS->_code_ir);
}
ll_define_primitive_end


ll_define_primitive(_ir, ___ir_code, _1(ir), _1(no_side_effect, "#t"))
{
  ll_return(ll_THIS->_code);
}
ll_define_primitive_end


ll_define_primitive(_ir, _ir_code, _1(ir), _1(no_side_effect, "#t"))
{
  ll_call_tail(ll_o(___ir_code), _1(ll_THIS->_code_ir));
}
ll_define_primitive_end


ll_define_primitive(_ir, ___ir_call_site_count, _1(ir), _1(no_side_effect, "#t"))
{
  ll_return(ll_THIS->_call_site_count);
}
ll_define_primitive_end


ll_define_primitive(_ir, _ir_call_site_count, _1(ir), _1(no_side_effect, "#t"))
{
  ll_call_tail(ll_o(___ir_call_site_count), _1(ll_THIS->_code_ir));
}
ll_define_primitive_end


ll_define_primitive(_ir, _ir_globals, _1(ir), _1(no_side_effect,"#t"))
{
  ll_call_tail(ll_o(vector_ref), _2(ll_THIS->_consts, ll_make_fixnum(0)));
}
ll_define_primitive_end


ll_define_primitive(_ir, _ir_global_bindings, _1(ir), _1(no_side_effect,"#t"))
{
  ll_call_tail(ll_o(vector_ref), _2(ll_THIS->_consts, ll_make_fixnum(1)));
}
ll_define_primitive_end


ll_define_primitive(_ir, _ir_consts, _1(ir), _1(no_side_effect,"#t"))
{
  ll_return(ll_THIS->_consts);
}
ll_define_primitive_end


ll_define_primitive(_ir, _ir_scope, _1(ir), _1(no_side_effect,"#t"))
{
  ll_return(ll_THIS->_scope);
}
ll_define_primitive_end


ll_define_primitive(_ir, _ir_formals, _1(ir), _1(no_side_effect,"#t"))
{
  ll_return(ll_THIS->_formals);
}
ll_define_primitive_end


ll_define_primitive(_ir, _ir_body, _1(ir), _1(no_side_effect,"#t"))
{
  ll_return(ll_THIS->_body);
}
ll_define_primitive_end


ll_define_primitive(_ir, _ir_code_gen, _1(ir), _1(no_side_effect,"#t"))
{
  ll_return(ll_THIS->_code_gen);
}
ll_define_primitive_end


ll_define_primitive(_ir, locative_properties, _1(ir), _1(no_side_effect,"#t"))
{
  if ( ! ll_unbox_boolean(ll_THIS->_properties) ) {
    if ( ll_unbox_boolean(ll_THIS->_parent) ) {
      ll_setf(ll_THIS, _properties, ll_call(ll_o(properties), _1(ll_THIS->_parent)));
    }
  }
  if ( ! ll_unbox_boolean(ll_THIS->_properties) ) {
    ll_setf(ll_THIS, _properties, ll_nil);
  }

  ll_return(ll_make_locative(&ll_THIS->_properties));
}
ll_define_primitive_end


ll_define_primitive(_ir, _ir_set_propE, _3(ir,prop,value), _0())
{
  _ll_assq_set(&ll_THIS->_props, ll_ARG_1, ll_ARG_2);
}
ll_define_primitive_end


ll_define_primitive(_ir,_ir_root_lambda, _1(ir), _1(no_side_effect,"#t"))
{
  if ( ll_unbox_boolean(ll_THIS->_car_posQ) ) {
    ll_call_tail(ll_o(_ir_root_lambda), _1(ll_THIS->_parent));
  } else {
    ll_return(IR);
  }
}
ll_define_primitive_end


ll_define_primitive(_ir, _ir_allocate_local, _1(ir), _0())
{
  if ( ll_unbox_boolean(ll_THIS->_car_posQ) ) {
    ll_call_tail(ll_o(_ir_allocate_local), _1(ll_call(ll_o(_ir_root_lambda), _1(IR))));
  } else {
    ll_v x;

    x = ll_THIS->_n_locals;
    ll_setf(ll_THIS, _n_locals, ll_call(ll_o(_ADD), _2(ll_THIS->_n_locals, ll_make_fixnum(1))));

    if ( ll_unbox_fixnum(ll_THIS->_max_n_locals) < ll_unbox_fixnum(ll_THIS->_n_locals) ) {
      ll_setf(ll_THIS, _max_n_locals, ll_THIS->_n_locals);
      ll_write_barrier_pure(ll_SELF);
    }

    ll_return(x);
  }
}
ll_define_primitive_end


ll_define_primitive(_ir, _ir_allocate_global, _2(ir,global), _0())
{
  ll_v globals = ll_call(ll_o(_ir_globals), _1(ll_SELF));
  ll_v x = ll_call(ll_o(vector_length), _1(globals));

  ll_call(ll_o(append_oneE), _2(globals, ll_ARG_1));

  globals = ll_call(ll_o(_ir_global_bindings), _1(ll_SELF));
  ll_call(ll_o(append_oneE), _2(globals, ll_f));

  ll_return(x);
}
ll_define_primitive_end


ll_define_primitive(_ir, _ir_allocate_export, _2(ir, var), _0())
{
  ll_v x = ll_ARG_1;

  if ( ll_unbox_boolean(ll_THIS->_car_posQ) ) {
    ll_v save_x = x;

    /*
    ** If we need to export a local from a sub car-pos lambda,
    ** allocate an anonymous export vector slot.
    */
    if ( ll_EQ(var_type(x), ll_s(local)) ) {
      x = ll_listn(6, ll_undef, ll_undef, ll_f, ll_f, ll_f, ll_f);
    }

    /* Ensure variable is allocated in parent. */
    ll_call(ll_o(_ir_var_scope), _2(IR, x));

    ll_call(ll_o(_ir_allocate_export), _2(ll_call(ll_o(_ir_root_lambda), _1(IR)), x));

    /* Copy the environment index back to our requested var */
    set_var_closed_overQ(save_x, var_closed_overQ(x)); 
  } else {
    /* Ensure variable is allocated. */
    ll_call(ll_o(_ir_var_scope), _2(IR, x));

    /* Allocate and remember a slot in our environment vector */
    set_var_closed_overQ(x, ll_THIS->_env_length);

    ll_THIS->_env_length = ll_call(ll_o(_ADD), _2(ll_THIS->_env_length, ll_make_fixnum(1)));
    ll_write_barrier_SELF();

    /* Append to the environment list */
    /*
    ** Keep the environment list in reverse environment vector order
    ** because the environment vector is built on the stack.  
    */
    ll_THIS->_env = ll_cons(x, ll_THIS->_env);
    ll_write_barrier_SELF();
  }
}
ll_define_primitive_end


ll_define_primitive(_ir, initialize, __2(ir, body, args), _0())
{
  ll_THIS->_body = ll_ARG_1;
  ll_THIS->_formals = ll_ARGC >= 3 ? ll_ARG_2 : ll_nil;
  ll_THIS->_slots = ll_ARGC >= 4 ? ll_ARG_3 : ll_nil;
  ll_THIS->_parent = ll_ARGC >= 5 ? ll_ARG_4 : ll_f;
  ll_THIS->_car_posQ = ll_ARGC >= 6 ? ll_ARG_5 : ll_f;
  ll_THIS->_properties = ll_nil;
  ll_THIS->_code_gen = ll_f;

  ll_THIS->_code_gen = ll_call(ll_o(make), _1(ll_type(mutable_vector)));

  ll_write_barrier_SELF();

  /*
  ** Create a properties list for the child method.
  ** e.g. documentation, debugging support, etc.
  */
  ll_THIS->_props = ll_listn(3,
			     ll_cons(ll_s(formals), ll_THIS->_formals),
			     ll_cons(ll_s(slots), ll_THIS->_slots),
			     ll_cons(ll_s(body), ll_THIS->_body)
			    );
  ll_write_barrier_SELF();

  /* Child IRs share constant vectors with root IR. */
  if ( ll_unbox_boolean(ll_THIS->_parent) ) {
    ll_v root_lambda = ll_call(ll_o(_ir_root_lambda), _1(ll_THIS->_parent));
    ll_THIS->_consts = ll_call(ll_o(_ir_consts), _1(root_lambda));
    ll_write_barrier_SELF();
  } else {
    ll_THIS->_consts = ll_call(ll_o(make), _1(ll_type(mutable_vector)));
    ll_write_barrier_SELF();

    /* globals */
    ll_call(ll_o(append_oneE), _2(ll_THIS->_consts, ll_call(ll_o(make), _1(ll_type(mutable_vector)))));

    /* global bindings */
    ll_call(ll_o(append_oneE), _2(ll_THIS->_consts, ll_call(ll_o(make), _1(ll_type(mutable_vector)))));
  }

  /* car-pos (inlined lambdas) share code vectors with root IR. */
  if ( ll_unbox_boolean(ll_THIS->_car_posQ) ) {
    ll_v root_lambda = ll_call(ll_o(_ir_root_lambda), _1(ll_THIS->_parent));
    ll_THIS->_code_ir = ll_call(ll_o(_ir_code_ir), _1(root_lambda));
    ll_THIS->_code = ll_undef;
    ll_THIS->_call_site_count = ll_undef;
  } else {
    ll_THIS->_code_ir = ll_SELF;
    ll_THIS->_code = ll_call(ll_o(make), _1(ll_type(mutable_string)));
    ll_THIS->_call_site_count = ll_make_fixnum(0);
  }

  ll_THIS->_scope = ll_nil;
  ll_THIS->_rest_argQ = ll_f;
  ll_THIS->_env = ll_nil;
  ll_THIS->_env_exportedQ = ll_f;
  ll_THIS->_imports = ll_nil;
  ll_write_barrier_SELF();

  ll_THIS->_env_length = ll_make_fixnum(0);
  ll_write_barrier_SELF();
  ll_THIS->_n_locals = ll_make_fixnum(0);
  ll_write_barrier_SELF();
  ll_THIS->_max_n_locals = ll_make_fixnum(0);
  ll_write_barrier_SELF();

  {
    ll_v x, entry;
    int i;

    /* Scan slots and add to scope */
    /* Do slots first so formals will override slots */
    i = 0;
    x = ll_THIS->_slots;
    while ( ! ll_nullQ(x) ) {
      ll_v var, type, index;

      var = ll_car(x);
      type = ll_s(slot);
      index = ll_make_fixnum(i);

      entry = ll_listn(6, var, type, index, ll_f, ll_f, ll_f);

      ll_THIS->_scope = ll_cons(entry, ll_THIS->_scope);
      ll_write_barrier_SELF();
      i ++;
      x = ll_cdr(x);
    }
    
    /* Scan formals and add to scope */

    /*
    ** IMPLEMENT: If there is only a rest-arg and a slot is referenced
    ** be sure for force checking for at least one argument, namely "self".
    */

    x = ll_THIS->_formals;
    i = 0;
    while ( ! ll_nullQ(x) ) {
      ll_v var, type, index;

      if ( ll_unbox_boolean(ll_call(ll_o(symbolQ), _1(x))) ) {
	/* a restarg is not really an arg but actually a local */
	ll_THIS->_rest_argQ = x;
	ll_write_barrier_SELF();

	var = x;
	type = ll_s(local);
	index = ll_f;
	x = ll_nil;
      } else {
	var = ll_car(x);
	/* If this lambda is car-position, the argument is actually
	** in a local slot, allocated by the root lambda.
	*/
	if ( ll_unbox_boolean(ll_THIS->_car_posQ) ) {
	  type = ll_s(local);
	  index = ll_f;
	} else {
	  type = ll_s(arg);
	  index = ll_make_fixnum(i);
	}
	x = ll_cdr(x);
	i ++;
      }

      entry = ll_listn(6, var, type, index, ll_f, ll_f, ll_THIS->_rest_argQ);
    
      ll_THIS->_scope = ll_cons(entry, ll_THIS->_scope);
      ll_write_barrier_SELF();
    }

    ll_THIS->_argc = ll_make_fixnum(i);
    ll_write_barrier_SELF();
  }

  /* Begin scoping and rewrite rules */
  ll_call(ll_o(_ir_compile1_body), _1(IR));

  ll_return(IR);
}
ll_define_primitive_end


/**********************************************************************/



ll_define_primitive(_ir, _ir_var_scope, __2(ir, var, close_over), _0())
{
  ll_v var = ll_ARG_1;
  ll_v x;
  ll_v type;
  ll_v index = ll_f;
  ll_v close_over = ll_ARGC > 2 ? ll_ARG_2 : ll_f;

  /* Already a var struct? */
  if ( ll_pairQ(var) ) {
    x = var;
  } else {
    /* Look up a scope in the cache */
    x = ll_assq(var, ll_THIS->_scope);
  }

  if ( ll_unbox_boolean(x) ) {
    type = var_type(x);

    /* If it's not a global */
    if ( ll_NE(type, ll_s(glo)) ) {
      /* Is going to be closed over? */
      if ( ll_unbox_boolean(close_over) ) {
	/* If not already closed over, */
	if ( ! ll_unbox_boolean(var_closed_overQ(x)) ) {
	  /* Allocate space in export vector. */
	  ll_call(ll_o(_ir_allocate_export), _2(ll_SELF, x));
	}
      } else if ( ll_EQ(var_type(x), ll_s(local)) && ! ll_unbox_boolean(var_referencedQ(x)) ) {
	/* If it's a local that has not been referenced yet, 
	 * allocate local space.
	 */
	ll_v index = ll_call(ll_o(_ir_allocate_local), _1(IR));
	set_var_index(x, index);
      }

      /* Mark variable as referenced. */
      set_var_referencedQ(x, ll_t);
    }
    
    ll_return(x);
  }

  /*
  ** Now look in parent:
  */
  if ( ll_unbox_boolean(ll_THIS->_parent) ) {
    /* In a car-pos lambda? */
    if ( ll_unbox_boolean(ll_THIS->_car_posQ) ) {
      /* Simply do a lookup without closing-over */
      x = ll_call(ll_o(_ir_var_scope), _3(ll_THIS->_parent, var, close_over));

    } else {
      ll_v co;

      /*
      ** Pass the close-over flag so we can force the defining scope to
      ** close-over it (put it into it's exported environment).
      */
      x = ll_call(ll_o(_ir_var_scope), _3(ll_THIS->_parent, var, ll_t));
      type = var_type(x);

      if ( ll_NE(type, ll_s(glo)) ) {
	/* Did our parent return a closed-over value? */
	co = var_closed_overQ(x);
	
	if ( ll_unbox_boolean(co) ) {
	  ll_v env_type;

	  /*
	   * Closed over args and locals are stored directly in the parent's
	   * exported environment.
	   *
	   * All others store a locative in the environment slot
	   * to the closed-over slot.
	   */
	  if ( ll_EQ(type, ll_s(arg)) || ll_EQ(type, ll_s(local)) ) {
	    env_type = ll_s(env);
	  } else {
	    env_type = ll_s(env_loc);
	  }
	  /*
	    ll_format(ll_undef, "  in parent x = ~S => ~S\n", 2, x, env_type);
	  */

	  /*
	  ** Scope it here as being in the environment vector that our parent
	  ** created for us.  But mark it as not yet closed-over by our
	  ** child lambdas.
	  */
	  x = ll_listn(6, var_name(x), env_type, co, ll_f, ll_t, ll_f);

	  /*
	  ll_format(ll_undef, "  in env ~S\n", 1, x);
	  */

	  ll_THIS->_imports = ll_cons(x, ll_THIS->_imports);
	  ll_write_barrier_SELF();
	}
      }
    }
  }

  /* Not found: must be global. */
  if ( ! ll_unbox_boolean(x) ) {
    index = ll_call(ll_o(_ir_allocate_global), _2(ll_SELF, var)); 
    x = ll_listn(6, var, ll_s(glo), index, ll_f, ll_t, ll_f);
  }

  /* Add to our scope list. */
  ll_THIS->_scope = ll_cons(x, ll_THIS->_scope);
  ll_write_barrier_SELF();

  ll_return(x);
}
ll_define_primitive_end


/************************************************************************/


/*****************************************************************
 * Low-level emit methods.
 */

ll_define_primitive(_ir, _ir_const_index, _2(ir, c), _0())
{
  int i;

  /* Search constant vector for an equal? member. */
  /* Skip over globals and global bindings. */
  i = 2;
  ll_VECTOR_LOOP_FROM(ll_THIS->_consts, x, i);
  {
    if ( 
	// ll_equalQ(ll_ARG_1, x)
	ll_EQ(ll_ARG_1, x)
	 )
      ll_return(ll_make_fixnum(i));
    i ++;
  }
  ll_VECTOR_LOOP_END;
  
  /* i == (length (%ir-consts ir)). */
  ll_call(ll_o(append_oneE), _2(ll_THIS->_consts, ll_ARG_1));

  /* ll_format(ll_undef, "~O %ir-const-index: ~S [~S]\n", 3, ll_SELF, ll_ARG_1, ll_make_fixnum(i)); */

  ll_return(ll_make_fixnum(i));
}
ll_define_primitive_end


ll_define_primitive(_ir, __ir_bc, _2(ir, c), _0())
{
  ll_v x;

  /* Find the byte-code for the symbolic name */
  x = _ll_assq_to_front(&ll_g(_bc_defs), ll_ARG_1);

  if ( ! ll_unbox_boolean(x) ) {
    _ll_error(ll_fe(bytecode), 1, ll_s(bytecode), ll_ARG_1);
  }
 
  ll_return(x);
}
ll_define_primitive_end


ll_define_primitive(_ir, ___ir_emit, _2(ir, c), _0())
{
  ll_v x = ll_call(ll_o(__ir_bc), _2(ll_SELF, ll_ARG_1));
  ll_v c = ll_cadr(x);
  int ch = ll_unbox_char(c);
  ll_assert_msg(general, ch > 0, ("code = %s bytecode = %d", ll_po(ll_ARG_1), ch));
  ll_call_tail(ll_o(append_oneE), _2(ll_THIS->_code, c));
}
ll_define_primitive_end


ll_define_primitive(_ir, ___ir_emit_pos, _1(ir), _1(no_side_effect, "#t"))
{
  ll_call_tail(ll_o(string_length), _1(ll_THIS->_code));
}
ll_define_primitive_end


static
unsigned char * encode_long(long x, unsigned char *buf)
{
  buf[0] =  x        & 0xff;
  buf[1] = (x >> 8)  & 0xff;
  buf[2] = (x >> 16) & 0xff;
  buf[3] = (x >> 24) & 0xff;
  
  return buf;
}


ll_define_primitive(_ir, ___ir_emit_with_int, _3(ir, c, i), _0())
{
  long x = ll_unbox_fixnum(ll_ARG_2);
  ll_v i;

  ll_assert_msg(ir, x >= 0, ("bad op argument value: %ld", (long) x));

  ll_call(ll_o(___ir_emit), _2(ll_SELF, ll_ARG_1));
  i = ll_call(ll_o(___ir_emit_pos), _1(ll_SELF));

  {
    unsigned char buf[4];

    encode_long(x, buf);

#if 0
    ll_call(ll_o(appendE), _2(ll_THIS->_code, _ll_string_make(buf, sizeof(buf))));
#else
    ll_call(ll_o(append_oneE), _2(ll_THIS->_code, ll_make_char(buf[0])));
    ll_call(ll_o(append_oneE), _2(ll_THIS->_code, ll_make_char(buf[1])));
    ll_call(ll_o(append_oneE), _2(ll_THIS->_code, ll_make_char(buf[2])));
    ll_call(ll_o(append_oneE), _2(ll_THIS->_code, ll_make_char(buf[3])));
#endif
  }

  ll_return(i);
}
ll_define_primitive_end


ll_define_primitive(_ir, ___ir_emit_patch, _3(ir, ploc, dloc), _0())
{
  size_t i = ll_unbox_fixnum(ll_ARG_1);
  long x = ll_unbox_fixnum(ll_ARG_2);
  unsigned char buf[4];

  encode_long(x, buf);

  ll_call(ll_o(string_setE), _3(ll_THIS->_code, ll_make_fixnum(i), ll_make_char(buf[0])));
  ll_call(ll_o(string_setE), _3(ll_THIS->_code, ll_make_fixnum(i + 1), ll_make_char(buf[1])));
  ll_call(ll_o(string_setE), _3(ll_THIS->_code, ll_make_fixnum(i + 2), ll_make_char(buf[2])));
  ll_call_tail(ll_o(string_setE), _3(ll_THIS->_code, ll_make_fixnum(i + 3), ll_make_char(buf[3])));
}
ll_define_primitive_end


/* Keep track of the number of call sites. */
ll_define_primitive(_ir, ___ir_emit_csite_check, _2(ir, c), _0())
{
  ll_v x = ll_call(ll_o(__ir_bc), _2(ll_SELF, ll_ARG_1));

  /* If its a call, increment the call site count. */
  if ( ll_unbox_boolean(ll_caddr(x)) ) {
    ll_v call_site_i = ll_THIS->_call_site_count;

    if ( 0 && ll_DEBUG(compile_emit) > 0 ) {      
      ll_format(ll_undef, "  ~O emiting csite-i for ~S\n", 2, ll_SELF, x);
    }

    ll_THIS->_call_site_count = ll__ADD(ll_THIS->_call_site_count, ll_make_fixnum(1)); 

    if ( ll_DEBUG(compile_emit) > 0 ) {      
      ll_format(ll_undef, "  ~O emiting csite-i ~S for ~S\n", 
		3,
		ll_SELF, 
		call_site_i,
		x
		);
    }

    ll_call(ll_o(___ir_emit_with_int), _3(ll_SELF, ll_s(csite_i), call_site_i));
  }
}
ll_define_primitive_end


/***************************************************
 * Emit methods that dispatch to code_ir
 */

ll_define_primitive(_ir, _ir_emit_code_gen, _2(ir, rep), _1(no_side_effect, "#f"))
{
  if ( ll_unbox_boolean(ll_THIS->_code_gen) ) {
    ll_call_tail(ll_o(append_oneE), _2(ll_THIS->_code_gen, ll_ARG_1));
  }
}
ll_define_primitive_end


ll_define_primitive(_ir, _ir_emit_pos, _1(ir), _1(no_side_effect, "#t"))
{
  ll_v pos = ll_call(ll_o(___ir_emit_pos), _1(ll_THIS->_code_ir));
  ll_v expr = ll_listn(2, ll_s(label), pos);
  ll_call(ll_o(_ir_emit_code_gen), _2(ll_THIS->_code_ir, expr));
  ll_return(pos);
}
ll_define_primitive_end


ll_define_primitive(_ir, __ir_emit, _2(ir, c), _0())
{
  ll_call_tail(ll_o(___ir_emit), _2(ll_THIS->_code_ir, ll_ARG_1));
}
ll_define_primitive_end


ll_define_primitive(_ir, __ir_emit_with_int, _3(ir, c, i), _0())
{
  ll_call_tail(ll_o(___ir_emit_with_int), _3(ll_THIS->_code_ir, ll_ARG_1, ll_ARG_2));
}
ll_define_primitive_end


/*****************************************************************
 * User level emit methods.
 */

ll_define_primitive(_ir, _ir_emit, _2(ir, c), _0())
{
  ll_call(ll_o(___ir_emit_csite_check), _2(ll_THIS->_code_ir, ll_ARG_1));
  
  {
    ll_v expr = ll_listn(1, ll_ARG_1);
    ll_call(ll_o(_ir_emit_code_gen), _2(ll_THIS->_code_ir, expr));
  }

  if ( ll_DEBUG(compile_emit) ) {
    ll_format(ll_undef, "~O: ~O: emit: ~S : ~S\n",
	      4, 
	      ll_SELF, 
	      ll_THIS->_code_ir,
	      ll_call(ll_o(_ir_emit_pos),  _1(ll_SELF)),
	      ll_ARG_1);
  }

  ll_call_tail(ll_o(__ir_emit), _2(ll_SELF, ll_ARG_1));
}
ll_define_primitive_end


ll_define_primitive(_ir, _ir_binding_i, _3(ir, type, i), _0())
{
  ll_v type = ll_ARG_1;
  
  if ( ll_EQ(type, ll_s(glo)) ||
       ll_EQ(type, ll_s(glo_set)) ||
       ll_EQ(type, ll_s(glo_loc))
       ) {
    type = ll_s(glo);
  }

  ll_LIST_LOOP(ll_THIS->_scope, x); {
    if ( ll_EQ(type, ll_cadr(x)) && ll_EQ(ll_ARG_2, ll_caddr(x)) ) {
      ll_return(x);
    }
  }
  ll_LIST_LOOP_END;

  ll_return(ll_f);
}
ll_define_primitive_end


ll_define_primitive(_ir, _ir_emit_with_int, _3(ir, c, i), _0())
{
  ll_call(ll_o(___ir_emit_csite_check), _2(ll_THIS->_code_ir, ll_ARG_1));

  {
    ll_v x = ll_ARG_2;
    ll_v expr;
    
    if ( ll_EQ(ll_ARG_1, ll_s(glo)) ||
	 ll_EQ(ll_ARG_1, ll_s(glo_set)) ||
	 ll_EQ(ll_ARG_1, ll_s(glo_loc)) ||
	 ll_EQ(ll_ARG_1, ll_s(slot)) ||
	 ll_EQ(ll_ARG_1, ll_s(arg)) ||
	 ll_EQ(ll_ARG_1, ll_s(local))
	 ) {
      x = ll_call(ll_o(_ir_binding_i), _3(ll_SELF, ll_ARG_1, ll_ARG_2));
      if ( ll_unbox_boolean(x) ) {
	x = ll_car(x);
      }
      expr = ll_listn(3, ll_ARG_1, ll_ARG_2, x);
    } 
    else if ( ll_EQ(ll_ARG_1, ll_s(const)) ) {
      x = ll_call(ll_o(vector_ref), _2(ll_THIS->_consts, ll_ARG_2));
      expr = ll_listn(3, ll_ARG_1, ll_ARG_2, x);
    } 
    else {
      expr = ll_listn(2, ll_ARG_1, ll_ARG_2);
    }
   
    ll_call(ll_o(_ir_emit_code_gen), _2(ll_THIS->_code_ir, expr));
    
    if ( ll_DEBUG(compile_emit) ) {
      ll_format(ll_undef, "~O: ~O: emit: ~S : ~S ~S\n", 
		5, 
		ll_SELF,
		ll_THIS->_code_ir,
		ll_call(ll_o(_ir_emit_pos), _1(ll_SELF)), 
		ll_ARG_1, 
		x);
    } 
  }

  ll_call_tail(ll_o(__ir_emit_with_int), _3(ll_SELF, ll_ARG_1, ll_ARG_2));
}
ll_define_primitive_end


ll_define_primitive(_ir, _ir_emit_constant, _2(ir, c), _0())
{
  ll_v i = ll_call(ll_o(_ir_const_index), _2(ll_SELF, ll_ARG_1));
  ll_v expr;

  expr = ll_listn(3, ll_s(const), i, ll_ARG_1);
  ll_call(ll_o(_ir_emit_code_gen), _2(ll_THIS->_code_ir, expr));

  if ( ll_DEBUG(compile_emit) ) {
    ll_format(ll_undef, "~O: ~O: emit: ~S : '~S [~S]\n",
	      5, 
	      ll_SELF, 
	      ll_THIS->_code_ir,
	      ll_call(ll_o(_ir_emit_pos), _1(ll_SELF)), 
	      ll_ARG_1, 
	      i);
  }
  ll_call_tail(ll_o(__ir_emit_with_int), _3(ll_SELF, ll_s(const), i));
}
ll_define_primitive_end


ll_define_primitive(_ir, _ir_emit_patch, _3(ir, ploc, dloc), _0())
{
  size_t i = ll_unbox_fixnum(ll_ARG_1);
  long x = ll_unbox_fixnum(ll_ARG_2);

  if ( ll_DEBUG(compile_emit) ) {
    ll_format(ll_undef, "~O: ~O: emit-patch: ~S ~S : delta ~S\n", 
	      5, 
	      ll_SELF,
	      ll_THIS->_code_ir,
	      ll_ARG_1, 
	      ll_ARG_2, 
	      ll_make_fixnum(x));
  }

  ll_call_tail(ll_o(___ir_emit_patch), _3(ll_THIS->_code_ir, ll_ARG_1, ll_ARG_2));
}
ll_define_primitive_end


ll_define_primitive(_ir, _ir_emit_patch_relative, _3(ir, ploc, dloc), _0())
{
  size_t i = ll_unbox_fixnum(ll_ARG_1);
  long x = ll_unbox_fixnum(ll_ARG_2) - (i + 4);

  ll_call_tail(ll_o(_ir_emit_patch), _3(ll_SELF, ll_ARG_1, ll_make_fixnum(x)));
}
ll_define_primitive_end


#undef SELF
#undef IR
#undef CAR_POSQ


/**********************************************************************/

#define IR ll_SELF


ll_define_primitive(_ir, _ir_method_props, _1(ir), _0())
{
  /* Add the call-site-count to the method props. */
  ll_v x = ll_call(ll_o(_ir_call_site_count), _1(ll_SELF));

  x = ll_cons(
	      ll_cons(ll_s(call_site_count), 
		      x),
	      ll_THIS->_props
	      );

  if ( ll_DEBUG(compile_emit) > 1 ) {
    ll_format(ll_undef, "  ~O: ~O: call-site-count ~S\n",
	      3,
	      ll_SELF,
	      ll_THIS->_code_ir,
	      x);
  }

  ll_return(x);
}
ll_define_primitive_end


ll_define_primitive(_ir, _ir_method, _1(ir), _0())
{
  ll_v meth = ll_undef;

  if ( ll_DEBUG(compile_emit) > 1 ) {
    ll_format(ll_undef, "  %ir-method: before compile2: (ir ~S)\n", 1, ll_SELF);
  }

  /* Do constant folding. */
  if ( ! ll_unbox_boolean(ll_call(ll_o(property), _2(IR, ll_s(no_const_folding)))) ) {
    /* 
       ll_format(ll_undef, "const folding ~S\n", 1, ll_THIS->_body);
    */

    ll_call(ll_o(_ir_const_fold), _2((IR), (IR)));
    
    /*
      ll_format(ll_undef, "const folded ~S\n", 1, ll_THIS->_body);
    */
  } else {
    /* ll_format(ll_undef, "NO const folding ~S\n", 1, ll_THIS->_body); */
  }


  /**********************************************************************/
  /* Begin emiting code */

  ll_call(ll_o(_ir_compile2_body), _3(ll_SELF, ll_SELF, ll_t));
  
  /* abort(); */

  if ( ll_DEBUG(compile_emit) > 2 ) {  
    ll_format(ll_undef, "  %ir-method: after compile2: (ir ~S)\n", 1, ll_SELF);
  }

  /* ll_DEBUG(trace) = ll_make_fixnum(1); */

  /**********************************************************************/
  /* Create a method for the byte-code */

  
  meth = ll_call(ll_o(make), _4(ll_type(byte_code_method), 
				ll_call(ll_o(_ir_code), _1(ll_SELF)),
				ll_THIS->_consts,
				ll_call(ll_o(_ir_method_props), _1(ll_SELF))
				)
		 );

  /* ll_DEBUG(trace) = ll_make_fixnum(0); */

  ll_return(meth);
}
ll_define_primitive_end


ll_define_primitive(_ir,_ir_operation, _1(ir), _0())
{
  ll_v op, meth;

  op = ll_call(ll_o(make), _1(ll_type(operation)));

  meth = ll_call(ll_o(_ir_method), _1(ll_SELF));
  ll_call_tail(ll_o(_add_method), _3(ll_type(object), op, meth));
}
ll_define_primitive_end


/**********************************************************************/

ll_INIT(bcompile, 400, "bcompile")
{
  static struct {
    const char *name;
    int code;
  } x[] = {
#ifndef ll_bc_def
#define ll_bc_def(X,NARGS,SM,NSA) { #X, (int) ll_bc_##X },
#include "ll/bcs.h"
#endif
    { 0 }
  };
  int i;
  
  ll_g(_bc_defs) = ll_nil;
  
  for ( i = 0; x[i].name; i ++ ) {
    int is_call = strncmp(x[i].name, "call", 4) == 0;
    
    /* 
       fprintf(stderr, "ll_bc(%s) = %d, %s\n", x[i].name, x[i].code,
       is_call ? "call" : "");
    */
    ll_g(_bc_defs) = 
      ll_cons(
	      ll_listn(3,
		       _ll_make_symbol_escape(x[i].name), 
		       ll_make_char(x[i].code),
		       ll_make_boolean(is_call)
		       ), 
	      ll_g(_bc_defs));
  }


  return 0;
}


