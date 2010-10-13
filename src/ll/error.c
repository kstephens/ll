#include "ll.h"
#include <stdarg.h>
#include <signal.h> /* kill() */
#include <unistd.h> /* getpid() */


/* Add debugger breakpoint on this function. */
void 
_ll_abort_stop_at()
{
}

/* Add debugger breakpoint on this function. */
void 
_ll_error_stop_at()
{
}


/********************************************************************/


ll_define_primitive(error, initialize, __2(self, value, values), _0())
{
  ll_call_super(ll_o(initialize), ll_f, _1(ll_SELF));

  ll_THIS->_ar = ll_make_ref(_ll_ar_sp + 1); /* FIXME */
  ll_THIS->_properties = ll_nil;

  /* Collect props from arguments */
  {
    ll_v props = ll_nil;

    if ( ll_ARGC >= 3 ) {
      ll_v *xp = &props;
      size_t i;

      for ( i = 1; i < ll_ARGC - 1; i += 2 ) {
	xp = 
	  &ll_CDR(*xp = 
		  ll_cons(ll_cons(ll_ARGV[i], ll_ARGV[i + 1]), 
			  ll_nil));
      }
    } else {
      /* a list */
      props = ll_ARG_1;
    }

    ll_THIS->_properties = 
      ll_append(props, 
		ll_THIS->_properties); 
  }
  ll_write_barrier_SELF();

  /* Add the debug expr. */
  {
    extern ll_v *_ll_debug_expr;

    if ( _ll_debug_expr ) {
      ll_THIS->_properties = 
	ll_cons(
		ll_cons(ll_s(expr), *_ll_debug_expr),
		ll_THIS->_properties);
      ll_write_barrier_SELF();
    }
  }

  /* Add any additional properties. */
  {
    ll_v props = ll_fluid(ll_s(additional_error_properties));
    if ( ! ll_nullQ(props) ) {
      /* a list */
      ll_THIS->_properties = 
	ll_append(ll_THIS->_properties, props);
      ll_write_barrier_SELF();
    }
  }

  /* Scan for (ar . #<ar>) */
  ll_LIST_LOOP(ll_THIS->_properties, x);
  {
    /* Get the ar if specified. */
    if ( ll_EQ(ll_car(x), ll_s(ar)) ) {
      ll_THIS->_ar = ll_cdr(x);
      ll_write_barrier_SELF();
    }
  }
  ll_LIST_LOOP_END;

  //ll_format(ll_f, "<error>:initialize self->ar = ~S\n", 1, ll_THIS->_ar);
  
  ll_return(ll_SELF);
}
ll_define_primitive_end


ll_define_primitive(error, error_ar, _1(err), _1(no_side_effect, "#t"))
{
  ll_return(ll_THIS->_ar);
}
ll_define_primitive_end


ll_define_primitive(error, locative_properties, _1(err), _1(no_side_effect, "#t"))
{
  ll_return(ll_make_locative(&ll_THIS->_properties));
}
ll_define_primitive_end


ll_define_primitive(error, _write_properties, _2(obj, port), _0())
{
  ll_LIST_LOOP(ll_THIS->_properties, x);
  {
    ll_v key = ll_car(x);
    
    if ( ll_NE(key, ll_s(return_action)) ) {
      ll_format(ll_ARG_1, "  ~S => ~S\n", 2, ll_car(x), ll_cdr(x));
    }
  }
  ll_LIST_LOOP_END;
}
ll_define_primitive_end


ll_define_primitive(error, _write_default_contents, _3(obj, port, op), _0())
{
#if 0
  ll_tsa_message *ar = ll_THIS_ISA(message, ll_THIS->_ar);

  ll_format(ll_ARG_1, "op ~S", 1, ar->_op);
  if ( ar->_argc ) {
    ll_format(ll_ARG_1, " rcvr ~S", 1, ar->_argv[0]);
  }
#endif

  ll_format(ll_ARG_1, "\n", 0);

  ll_call_tail(ll_o(_write_properties), _2(ll_SELF, ll_ARG_1));
}
ll_define_primitive_end


/****************************************************************/
/* Error handler methods */


ll_define_primitive(error, error_start_debugger, _1(self), _0())
{
  ll_v result = ll_undef;

  ll_let_fluid();
  {
    ll_bind_fluid(ll_s(current_error), ll_SELF);

    /* ll_DEBUG_SET(trace, ll_make_fixnum(2)); */
    fflush(stdout);
    ll_format(ll_f, "\nll: ~S\n~S\n", 2, ll_TYPE(ll_SELF), ll_SELF);
    result = ll_call(ll_o(debugger), _1(ll_THIS->_ar));
  }
  ll_let_fluid_END();

  ll_return(result);
}
ll_define_primitive_end


ll_define_primitive(error, error_abort, _1(self), _0())
{
  fflush(stdout);
  fflush(stderr);
  ll_format(ll_f, "\nll: abort: ~S\n~S\n", 2, ll_TYPE(ll_SELF), ll_SELF);
  ll_abort("<error>:error-abort");
}
ll_define_primitive_end


ll_define_primitive(error, _handle_error, _1(self), _0())
{
  ll_v proc = ll_fluid(ll_s(error_handler));
  // ll_format(ll_f, "  using error-handler ~S\n", 1, proc);
  ll_call_tail(proc, _1(ll_SELF));
}
ll_define_primitive_end


ll_define_primitive(error, handle_error, _1(self), _0())
{
  /* The default for handle-error is print backtrace, envoke the debugger and escape to top-level read-eval-print loop */
  ll_v result = ll_call(ll_o(_handle_error), _1(ll_SELF));
  ll_call_tail(ll_o(top_level), _1(result));
}
ll_define_primitive_end


ll_define_primitive(recoverable_error, handle_error, _1(self), _0())
{
  /* Recoverable errors print backtrace, and tail-call the debugger. */
  ll_call_tail(ll_o(_handle_error), _1(ll_SELF));
}
ll_define_primitive_end


ll_define_primitive(list, additional_error_properties, _1(self), _0())
{
  ll_bind_fluid(ll_s(additional_error_properties),
		ll_append(ll_fluid(ll_s(additional_error_properties)),
			  ll_SELF));
}
ll_define_primitive_end


void _ll_additional_error_properties(int n, ...)
{
  va_list vap;
  ll_v props = ll_nil;
  ll_v *xp = &props;

  va_start(vap, n);

  {
    int i;

    for ( i = 0; i < n; ++ i ) {
      ll_v key = va_arg(vap, ll_v);
      ll_v val = va_arg(vap, ll_v);

      xp = 
	&ll_CDR(*xp = 
		ll_cons(ll_cons(key, val), 
			ll_nil));
      }
 
  }

  ll_call(ll_o(additional_error_properties), _1(props));

  va_end(vap);
}


void _ll_abort(const char *file, int lineno, const char *msg)
{
  static int aborting;

  if ( ! aborting ) {
    ++ aborting;
    fprintf(stderr, "\nll: abort: %s:%d: %s\n", file, lineno, msg);
    ll_call(ll_o(print_backtrace), _1(ll_make_ref(_ll_ar_sp)));
  }
  fprintf(stderr, "\nll: aborting\n");

  _ll_abort_stop_at();
  // exit(-1);
#ifdef SIGILL
  kill(getpid(), SIGILL);
#endif
  abort();
}


ll_define_primitive(fatal_error, handle_error, _1(self), _0())
{
  static int aborting = 0;

  if ( ! aborting ) {
    /* Fatal errors print themselves and abort() */
    ll_v type = ll_TYPE(ll_SELF);
    ll_v values = ll_THIS->super_error._properties;
    
    ++ aborting;

    fflush(stdout);
    fprintf(stderr, "\n\nll: Aborting in <fatal_error>:handle-error.\n");
    fprintf(stderr, "ll: FATAL error: %s", ll_po(type));
    fflush(stderr);
    
    while ( ! ll_nullQ(values)) {
      ll_v v = ll_car(values);
      ll_v key = ll_car(v);
      ll_v value = ll_cadr(v);
      values = ll_cdr(values);
      fprintf(stderr, "\n  %s %s", ll_po(key), ll_po(value));
      fflush(stderr);
    }
    fprintf(stderr, "\n");
    fflush(stderr);
    
    ll_call(ll_o(print_backtrace), _1(ll_make_ref(_ll_ar_sp)));
  }

  ll_abort("<fatal-error>:handle-error");
}
ll_define_primitive_end


/************************************************************************/


ll_v _ll_errorv(ll_v type, int argc, const ll_v *argv)
{
  ll_v rtn = ll_undef;
  int n;
  ll_v values = ll_nil, *valuesr = &values;

#if 0
  fprintf(stderr, "\n  _ll_errorv(%s, %d, %p)\n",
	  ll_po(type),
	  argc,
	  (void*) argv);
  for ( n = 0; n < argc; ++ n ) {
    fprintf(stderr, "  [%d] %s\n", n, ll_po(argv[n]));
  }
#endif

  _ll_error_stop_at();

  if ( ! ll_initializing && ll_EQ(ll_g(__building_internal_error), ll_f) ) {
    /* NOT_THREAD_SAFE */
    ll_set_g(__building_internal_error, ll_t);

    /* Create an assocation list of values for the error */
#define APPEND(key,value) \
    valuesr = &ll_CDR(*valuesr = ll_cons(ll_cons(key, value), ll_nil))

    for ( n = 0; n < argc; ++ n ) {
      ll_v key = argv[n ++];
      ll_v val = argv[n];
      APPEND(key, val);
    }

#undef APPEND

    rtn = ll_call(ll_o(make), _2(type, values));

    /* NOT_THREAD_SAFE */
    ll_set_g(__building_internal_error, ll_f);

    rtn = ll_call(ll_o(handle_error), _1(rtn));

    return(rtn);
  }

  /* Must have generated an error while creating and error object */
  fflush(stdout);
  fprintf(stderr, "\n\nll: Aborting in C _ll_error().\n");
  fprintf(stderr, "ll: FATAL error: %s op %s rcvr-type %s", ll_po(type), ll_po(ll_AR_OP), ll_po(ll_AR_TYPE));

  for ( n = 0; n < argc; ) {
    ll_v key = argv[n ++];
    ll_v val = argv[n ++];

    fprintf(stderr, "\n  %s %s", ll_po(key), ll_po(val));
  }
  fprintf(stderr, "\n");

  ll_abort("_ll_errorv()");

  return(rtn);
}


ll_v _ll_error(ll_v type, int argc, ...)
{
  ll_v *argv = alloca(sizeof(argv[0]) * (argc * 2 + 1));
  va_list vap;

  va_start(vap, argc);
  {
    int i = 0;
    int n;
    
    for ( n = 0; n < argc; ++ n ) {
      argv[i ++] = va_arg(vap, ll_v);
      argv[i ++] = va_arg(vap, ll_v);
    }
    
  }
  va_end(vap);
    
  return _ll_errorv(type, argc * 2, argv);
}


ll_define_primitive(object, raise, __1(type, props), _0())
{
  ll_return(_ll_errorv(ll_SELF, ll_ARGC - 1, ll_ARGV + 1));
}
ll_define_primitive_end


/****************************************************************/

/* Common error constructors */

ll_v _ll_argc_error()
{
  ll_v meth = ll_AR_METH;
  /*
  ll_v caller_op = _ll_ar_sp[1]._op;
  ll_v caller_meth = _ll_ar_sp[1]._meth;
  */

  return(_ll_error(
		   ll_re(arg_count), 
		   5,
		   ll_s(op), ll_AR_OP,
		   ll_s(meth), meth,
		   ll_s(formals), ll_call(ll_o(method_formals), _1(meth)),
		   ll_s(expected_argc), ll_call(ll_o(method_minargc), _1(meth)),
		   ll_s(called_argc), ll_make_fixnum(ll_ARGC)
		   ));
}


ll_v _ll_range_error(ll_v name, ll_v value, long low, long high)
{
  ll_v range = ll_cons(ll_make_fixnum(low), ll_make_fixnum(high));
  return(_ll_error(ll_re(range), 
		   4, 
		   ll_s(name), name, 
		   ll_s(value), value, 
		   ll_s(range), range,
		   ll_s(return_action), ll_listn(2, 
						 ll_make_string("specify a value within the range", -1), 
						 range)
		   ));
}


ll_v _ll_rangecheck(ll_v name, ll_v *value, long low, long high)
{
  while ( ! (ll_ISA_fixnum(*value) && 
	     low <= ll_UNBOX_fixnum(*value) && ll_UNBOX_fixnum(*value) <= high) ) { 
    *value = _ll_range_error(name, *value, low, high);
  }

  return(*value);
}


ll_v _ll_undefined_variable_error(ll_v name)
{
  return(_ll_error(ll_re(undefined_variable), 
		   3, 
		   ll_s(name), name,
		   ll_s(return_action), ll_make_string("specify a variable name to get.", -1),
		   ll_s(return_default), name
		   ));
}


ll_v _ll_readonly_variable_error(ll_v name)
{
  return(_ll_error(ll_re(readonly_variable), 
		   2, 
		   ll_s(name), name,
		   ll_s(return_action), ll_make_string("specify a variable name to modify.", -1)
		   ));
}


ll_v _ll_typecheck_error(ll_v type, ll_v value)
{
  return(_ll_error(ll_re(typecheck), 
		   3,
		   ll_s(type), type,
		   ll_s(value), value,
		   ll_s(return_action), ll_listn(2, ll_make_string("specify a value of the type ", -1), type)
		   ));
}


/* typecheck */
ll_define_primitive(object, _bad_typecheck, _2(self, type), _0())
{
  ll_return(_ll_typecheck_error(ll_ARG_1, ll_ARG_0));
}
ll_define_primitive_end


__inline
ll_v _ll_typecheck(ll_v type, ll_v *valuep)
{
  ll_v value_type = ll_TYPE(*valuep);
  /* Does not match type exactly? */
  if ( ll_NE(value_type, type) ) {
    /* Invoke the type's typechecker, if not still initializing. */
    if ( ll_initialized ) {
      *valuep = (ll_call(ll_type_typechecker(type), _2(*valuep, type)));
    }
  }
  /* Return the value. */
  return *valuep;
}


ll_v _ll_typecheck_procedure(ll_v *valuep)
{
  ll_v value_type = ll_TYPE(*valuep);

  /* Quick short-circuit on known procedure subclasses */
  if ( ! (ll_EQ(value_type, ll_type(operation)) ||
	  ll_EQ(value_type, ll_type(settable_operation)) ||
	  ll_EQ(value_type, ll_type(locatable_operation))
	  )
       ) {
	 _ll_typecheck(ll_type(procedure), valuep); 
       }

  return *valuep;
}

/***********************************************************************/

ll_INIT(error, 280, "(fluid current-error), (fluid additional-error-properties)")
{
  ll_set_g(__building_internal_error, ll_f); 
  ll_bind_fluid(ll_s(current_error), ll_f);
  ll_bind_fluid(ll_s(additional_error_properties), ll_nil);
#if 0
  ll_bind_fluid(ll_s(error_handler), ll_o(error_start_debugger));
#else
  ll_bind_fluid(ll_s(error_handler), ll_o(error_abort));
#endif

  return 0;
}

/***********************************************************************/
