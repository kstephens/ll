#ifndef __rcs_id__
#ifndef __rcs_id_ll_env_c__
#define __rcs_id_ll_env_c__
static const char __rcs_id_ll_env_c[] = "$Id: env.c,v 1.34 2008/05/24 20:44:08 stephens Exp $";
#endif
#endif /* __rcs_id__ */

#include "ll.h"
#include "assert.h"
#include <string.h> /* memset() */


/***********************************************************************/
/* ptrs into the environment vectors */

/* See globals.c */
#include "ll/global_vars.h"

#if ll_GLOBAL_VARS
/* The current global bindings vector. */
int _ll_env_bindings_n = ll_gi_N;
ll_v *_ll_env_bindings;

/* The current global environment */
ll_v _ll_env; 
#endif

/***********************************************************************/


static ll_v _uninit; /* An uninitialized value. */

ll_v _ll_global_get(ll_v binding)
{
  ll_tsa_binding *b = ll_THIS_ISA(binding, binding);
  
  if ( ll_NE(b->super_object._isa, _uninit) ) { 
    if ( ll_EQ(b->_locative, ll_f) ) {
      _ll_undefined_variable_error(b->_symbol);
      fprintf(stderr, "\nll: FATAL: giving up in _ll_g_get()!\n");
      abort();
    }
  }

  if ( ll_unbox_boolean(b->_locative) ) {
    return *ll_unbox_locative(b->_locative);
  } else {
    return _ll_undefined_variable_error(b->_symbol);
  }
}


ll_v _ll_global_set(ll_v binding, ll_v value)
{
  ll_tsa_binding *b = ll_THIS_ISA(binding, binding);

  if ( ! ll_unbox_boolean(b->_locative) ) {
    b->_locative = ll_make_locative(&b->_value);
    ll_write_barrier_pure(binding);
  }
  /* fprintf(stderr, "_ll_global_set(%s, %s)\n", ll_po(b->_symbol), ll_po(value)); */

  if ( ll_unbox_boolean(b->_locative) ) {
    *ll_unbox_locative(b->_locative) = value;
    ll_write_barrier(ll_unbox_locative(b->_locative));
  } else {
    value = _ll_undefined_variable_error(b->_symbol);
  }

  return value;
}


/***********************************************************************/


ll_define_primitive(environment, initialize, _1(env), _0())
{
  ll_return(ll_SELF);
}
ll_define_primitive_end


/***********************************************************************/


ll_define_primitive(environment, bindings, _1(env), _1(no_side_effect,"#t"))
{
  ll_return(ll_THIS->_bindings);
}
ll_define_primitive_end


ll_define_primitive(environment, set_bindingsE, _2(env, bindings), _0())
{
  ll_THIS->_bindings = ll_ARG_1;
  ll_write_barrier_SELF();
  ll_return(ll_THIS->_bindings);
}
ll_define_primitive_end


/**********************************************************************/


ll_define_primitive(environment, clone, _1(env), _0())
{
  ll_v x;

  x = ll_call_super(ll_o(clone), ll_f, _1(ll_SELF));
  ll_call(ll_o(set_bindingsE), _2(x ,ll_call(ll_o(clone), _1(ll_THIS->_bindings))));

  ll_return(x);
}
ll_define_primitive_end


/**********************************************************************/


ll_define_primitive(object, SenvironmentS, _0(), _1(no_side_effect,"#t"))
{
  ll_return(_ll_env);
}
ll_define_primitive_end


ll_define_primitive(environment, Sset_environmentS, _1(env), _0())
{
  ll_v x;

  _ll_env_bindings = ll_ptr_vector(ll_THIS->_bindings);
  _ll_env_bindings_n =  ll_len_vector(ll_THIS->_bindings);

  x = _ll_env;
  _ll_env = ll_SELF;

  ll_return(x);
}
ll_define_primitive_end


/**********************************************************************/


ll_define_primitive(environment, _binding, __2(env, var, need_locative), _1(no_side_effect,"#t"))
{
  /* Lookup a binding or return #f. */
  ll_v need_locative = ll_unbox_boolean(ll_ARGC > 2 ? ll_ARG_2 : ll_f);
  ll_v bindings = ll_THIS->_bindings;
  size_t i, l = ll_len_vector(bindings);
  ll_v *v = ll_ptr_vector(bindings);

  for ( i = 0; i < l; i ++ ) {
    /* fprintf(stderr, "_binding_index: %3d %s(%ld) %s(%ld)\n", i, ll_po(v[i]), (unsigned long) v[i],  ll_po(ll_ARG_1), (unsigned long) ll_ARG_1); */
    ll_tsa_binding *b = ll_THIS_ISA(binding, v[i]);
    if ( ll_EQ(b->_symbol, ll_ARG_1) ) {
      if ( need_locative && ! ll_unbox_boolean(b->_locative) ) {
	ll_return(ll_f);
      }
      ll_return(v[i]);
    }
  }

  ll_return(ll_f);
}
ll_define_primitive_end


ll_define_primitive(environment, _binding_or_error, __2(env, var, need_locative), _1(no_side_effect, "#t"))
{
  ll_v need_locative = ll_ARGC > 2 ? ll_ARG_2 : ll_f;
  do {
    ll_v binding = ll_call(ll_o(_binding), _3(ll_SELF, ll_ARG_1, need_locative));
    if ( ll_unbox_boolean(binding) ) {
      ll_return(binding);
    } else {
      ll_ARG_1 = _ll_undefined_variable_error(ll_ARG_1);
    }
  } while ( 1 );
}
ll_define_primitive_end


ll_define_primitive(environment, _bind, _2(env, var), _0())
{
  /* Lookup a binding or create one */
  ll_v binding = ll_call(ll_o(_binding), _2(ll_SELF, ll_ARG_1));

  /* Didn't find one?  Create one. */
  if ( ll_EQ(binding, ll_f) ) {
    ll_v bindings = ll_THIS->_bindings;
    ll_v index;
    
    binding = ll_call(ll_o(make), _2(ll_type(binding), ll_ARG_1));

    index = ll_call(ll_o(vector_length), _1(bindings));
    ll_call(ll_o(append_oneE), _2(bindings, binding));

    /* A hack to force this environment to become the current environment */
    if ( ll_EQ(_ll_env, ll_SELF) ) {
      ll_call(ll_o(Sset_environmentS), _1(ll_SELF));
    }
  }

  ll_return(binding);
}
ll_define_primitive_end


/**********************************************************************/


ll_define_primitive(environment, symbol_properties, _2(env, symbol), _1(no_side_effect,"#t"))
{
  ll_v binding = ll_call(ll_o(_binding), _2(ll_SELF, ll_ARG_1));
  if ( ll_unbox_boolean(binding) ) {
    ll_call_tail(ll_o(properties), _1(binding));
  } else {
    ll_return(ll_nil);
  }
}
ll_define_primitive_end


ll_define_primitive(environment, set_symbol_propertiesE, _3(env, symbol, plist), _0())
{
  ll_v binding = ll_call(ll_o(_bind), _2(ll_SELF, ll_ARG_1));
  ll_call_tail(ll_o(set_propertiesE), _2(binding, ll_ARG_2));
}
ll_define_primitive_end


ll_define_primitive(environment, symbol_property, _3(env, symbol, prop), _1(no_side_effect,"#t"))
{
  ll_v binding = ll_call(ll_o(_binding), _2(ll_SELF, ll_ARG_1));
  if ( ll_unbox_boolean(binding) ) {
    ll_call_tail(ll_o(property), _2(binding, ll_ARG_2));
  } else {
    ll_return(ll_f);
  }
}
ll_define_primitive_end


ll_define_primitive(environment, set_symbol_propertyE, _4(env, symbol, prop, value), _0())
{
  ll_v binding = ll_call(ll_o(_bind), _2(ll_SELF, ll_ARG_1));
  ll_call_tail(ll_o(set_propertyE), _3(binding, ll_ARG_2, ll_ARG_3));
}
ll_define_primitive_end


/**********************************************************************/


ll_define_primitive(environment, make_readonly, _2(env, var), _0())
{
  ll_v binding = ll_call(ll_o(_bind), _2(ll_SELF, ll_ARG_1));
  ll_call_tail(ll_o(make_readonlyE), _1(binding));
}
ll_define_primitive_end


/********************************************************************/


ll_define_primitive(environment, _define, __3(env, var, value, doc), _0())
{
  do {
    ll_v binding;
    
    while ( binding = ll_call(ll_o(_bind), _2(ll_SELF, ll_ARG_1)), 
	    ll_unbox_boolean(ll_call(ll_o(readonlyQ), _1(binding))) ) {
      ll_ARG_1 = _ll_readonly_variable_error(ll_ARG_1);
    }

    /* If a doc string is specified, add it to the object */
    if ( ll_ARGC >= 4 ) {
      ll_call(ll_o(set_llCdocE), _2(ll_ARG_1, ll_ARG_3));
      ll_call(ll_o(set_llCdocE), _2(ll_ARG_2, ll_ARG_3));
    }

    ll_call_tail(ll_o(set_binding_valueE), _2(binding, ll_ARG_2));
  } while ( 1 );
}
ll_define_primitive_end


ll_define_primitive(environment, _get, _2(env, var), _1(no_side_effect,"#t"))
{
  ll_v binding = ll_call(ll_o(_binding_or_error), _2(ll_SELF, ll_ARG_1));
  ll_call_tail(ll_o(binding_value), _1(binding));
}
ll_define_primitive_end


ll_define_primitive(environment, _setE, _3(env, var, value), _0())
{
  do {
    ll_v binding;
    
    while ( binding = ll_call(ll_o(_binding_or_error), _2(ll_SELF, ll_ARG_1)),
	    ll_unbox_boolean(ll_call(ll_o(readonlyQ), _1(binding))) ) {
      ll_ARG_1 = _ll_readonly_variable_error(ll_ARG_1);
    }

    ll_call_tail(ll_o(_set_binding_valueE), _2(binding, ll_ARG_2));
  } while ( 1 );
}
ll_define_primitive_end


ll_define_primitive(environment,_undefine, _2(env, var), _0())
{
  ll_call_tail(ll_o(_setE), _3(ll_SELF, ll_ARG_1, ll_undef));
}
ll_define_primitive_end


ll_define_primitive(environment,_make_locative, _2(env, var), _0())
{
  ll_v binding = ll_call(ll_o(_binding_or_error), _2(ll_SELF, ll_ARG_1));

  ll_call_tail(ll_o(locative_binding_value), _1(binding));
}
ll_define_primitive_end


/*********************************************************************/


ll_define_primitive(environment, _define_macro, _3(env, var, proc), _0())
{
  do {
    ll_v binding;

#if 1
    while ( binding = ll_call(ll_o(_bind), _2(ll_SELF, ll_ARG_1)),
	    ll_unbox_boolean(ll_call(ll_o(readonlyQ), _1(binding))) ) {
      ll_ARG_1 = _ll_readonly_variable_error(ll_ARG_1);
    }
#endif
    
    ll_call_tail(ll_o(set_binding_macroE), _2(binding, ll_ARG_2));
  } while ( 1 );
}
ll_define_primitive_end


ll_define_primitive(environment, _macro, _2(env, var), _1(no_side_effect,"#t"))
{
  ll_v binding = ll_call(ll_o(_binding), _2(ll_SELF, ll_ARG_1));
  if ( ll_unbox_boolean(binding) ) {
    ll_call_tail(ll_o(binding_macro), _1(binding));
  } else {
    ll_return(ll_f);
  }
}
ll_define_primitive_end


/********************************************************************/


ll_define_primitive(symbol, _binding, _1(var), _0())
{
  ll_call_tail(ll_o(_binding), _2(_ll_env, ll_SELF));
}
ll_define_primitive_end


ll_define_primitive(symbol, _get, _1(var), _0())
{
  ll_call_tail(ll_o(_get), _2(_ll_env, ll_SELF));
}
ll_define_primitive_end


ll_define_primitive(symbol, _define, __2(var, value, doc), _0())
{
  if ( ll_ARGC >= 3 ) {
    ll_call_tail(ll_o(_define), _4(_ll_env, ll_SELF, ll_ARG_1, ll_ARG_2));
  } else {
    ll_call_tail(ll_o(_define), _3(_ll_env, ll_SELF, ll_ARG_1));
  }
}
ll_define_primitive_end


ll_define_primitive(symbol, _macro, _1(var), _0())
{
  ll_call_tail(ll_o(_macro), _2(_ll_env, ll_SELF));
}
ll_define_primitive_end


ll_define_primitive(symbol, _setE, _2(var, value), _0())
{
  ll_call_tail(ll_o(_setE), _3(_ll_env, ll_SELF, ll_ARG_1));
}
ll_define_primitive_end


ll_define_primitive(symbol, _make_locative, _1(symbol), _0())
{
  ll_call_tail(ll_o(_make_locative), _2(_ll_env, ll_SELF));
}
ll_define_primitive_end


/*******************************************************************/

static
ll_v _ll_uninit_binding(ll_v b, int clear)
{
  ll_v *x = ll_UNBOX_ref(b);
  int i;
  
  for ( i = 0; i < sizeof(ll_tsa_binding) / sizeof(ll_v); ++ i ) {
    if ( clear || ! ll_INITIALIZED(x[i]) ) {
      x[i] = _uninit;
    }
  }

  return b;
}


static
ll_v _ll_make_raw_binding()
{
  ll_v *x = ll_malloc(sizeof(ll_tsa_binding));
  ll_v b = ll_make_ref(x);
  ll_tsa_binding *bp = (void*) x;

  /* Mark binding slots as uninitialized. */
  _ll_uninit_binding(b, 1);

  /* bind binding to value slot. */
  bp->_locative = ll_BOX_locative(&bp->_value);
  
  ll_write_barrier_ptr_pure(bp);

  return b;
}


ll_INIT(env1, 10, "create a raw environment")
{
  int i;

  /* Fill it with some bogus value */
  _uninit = ll_BOX_locative(0);

  _ll_env_bindings = ll_malloc(sizeof(_ll_env_bindings[0]) * ll_gi_N);
  for ( i = 0; i < ll_gi_N; ++ i) {
    _ll_binding_init *gvi = _ll_gbi[i];
    ll_v x;

    x = 
      _ll_env_bindings[i] = 
      gvi && gvi->_bp ?
      _ll_uninit_binding(ll_make_ref(gvi->_bp), 0) :
        _ll_make_raw_binding();
    
#if ll_GLOBAL_VARS
    ll_tsa_binding *b = ll_THIS_ISA(binding, x);
    b->_locative = ll_BOX_locative(gvi->_gvp);
    ll_write_barrier_ptr_pure(b);

    if ( i != 0 ) {
      ll_assert(init, gvi->_gvp);
      *gvi->_gvp = _uninit;
    }

    if ( 0 /* ll_DEBUG(init_env) */ ) {
      fprintf(stderr, "\n  %s %p", gvi->_name, gvi->_gvp);
    }
#endif
    ll_write_barrier_ptr_pure(_ll_env_bindings);
  }

  return 0;
}


ll_INIT(env2, 115, "init globals to undef")
{
  int i;

  ll_assert_ref(ll_undef);
  ll_assert_ref(ll_t);
  ll_assert_ref(ll_f);
  ll_assert_ref(ll_nil);

  /* Fill in undefined binding values for environment */
  for ( i = 0; i < ll_gi_N; ++ i ) {
    ll_v x = _ll_env_bindings[i];
    ll_tsa_binding *b = ll_THIS_ISA(binding, x);

    /* Set binding's type */
    ll_TYPE_ref(x) = ll_type(binding);
    ll_write_barrier(x);

    /* Not readonly, yet! */
    b->_readonlyQ = ll_f;
    // if ( ll_EQ(b->_readonlyQ, _uninit) ) {
    // }

    if ( ll_EQ(b->_locative, _uninit) ) {
      /* ll_warn_msg(env,!"binding locative changed", ("binding %s: was %s", _ll_gn(i), ll_po_(b->_locative))); */

      b->_locative = ll_make_locative(&b->_value);
      ll_write_barrier(x);
    }

    if ( 0 ) {
      fprintf(stderr, "  binding '%s' %d\n", _ll_gn(i), i);
    }
    ll_assert_locative(b->_locative);
    if ( i != 0 ) {
      ll_assert_msg(init, ll_UNBOX_locative(b->_locative), ("binding %s: locative == NULL", _ll_gn(i)));

      if ( ll_EQ(*ll_UNBOX_locative(b->_locative), _uninit) ) {
	/* ll_warn_msg(env,!"binding *loc changed", ("binding %s: was %s", _ll_gn(i), ll_po_(*ll_UNBOX_locative(b->_locative)))); */
	
	*ll_UNBOX_locative(b->_locative) = ll_undef;
	ll_write_barrier(x);
      }
    } else {
      b->_locative = ll_BOX_locative(&b->_value);
    }

    if ( ll_EQ(b->_value, _uninit) ) {
      /* ll_warn_msg(env,!"binding value changed", ("binding %s: was %s", _ll_gn(i), ll_po_(b->_value))); */

      b->_value = ll_undef;
      ll_write_barrier(x);
    }

    if ( ll_EQ(b->_macro, _uninit) ) {
      /* ll_warn_msg(env,!"binding macro changed", ("binding %s", _ll_gn(i))); */

      b->_macro = ll_f;
      ll_write_barrier(x);
    }    

    if ( ll_EQ(b->_properties, _uninit) ) {
      /* ll_warn_msg(env,!"binding properties changed", ("binding %s", _ll_gn(i))); */

      b->_properties = ll_nil;
      ll_write_barrier(x);
    }    
  }

  return 0;
}


ll_INIT(env3, 160, "add the <symbol> objects to the environment")
{
  int i;

  /* Fill in symbol in the binding */
  for ( i = 0; i < ll_gi_N; ++ i ) {
    ll_v sym = _ll_make_symbol_escape(_ll_gn(i));
    ll_v b = _ll_env_bindings[i];
    ll_THIS_ISA(binding, b)->_symbol = sym;
    ll_write_barrier(b);
  }

  return 0;
}


ll_INIT(env4, 250, "create the <environment> object")
{
  /* Create an environment */
  _ll_env = ll_call(ll_o(make), _1(ll_type(environment)));

  ll_call(ll_o(set_bindingsE), _2(_ll_env, ll_make_vector(_ll_env_bindings, ll_gi_N)));

  ll_call(ll_o(Sset_environmentS), _1(_ll_env));

  return 0;
}


/*********************************************************************/

