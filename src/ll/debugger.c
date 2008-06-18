#ifndef __rcs_id__
#ifndef __rcs_id_ll_debugger_c__
#define __rcs_id_ll_debugger_c__
static const char __rcs_id_ll_debugger_c[] = "$Id: debugger.c,v 1.23 2008/05/26 06:35:03 stephens Exp $";
#endif
#endif /* __rcs_id__ */

#include "ll.h"

/* a simple debugger */


/*********************************************************************/


ll_define_primitive(debugger, locative_properties, _1(db), _1(no_side_effect,"#t"))
{
  ll_return(ll_make_locative(&ll_THIS->_properties));
}
ll_define_primitive_end


/* (initialize <db> <ar>? <props>? <error>?) */
ll_define_primitive(debugger, initialize, __1(db, args), _0())
{
  ll_v ar = ll_ARGC >= 2 ? ll_ARG_1 : ll_make_ref(_ll_ar_sp + 1);

  ll_THIS->_top_of_ar_stack = ar;
  ll_THIS->_current_ar = ll_THIS->_top_of_ar_stack;
  ll_THIS->_properties = ll_ARGC >= 3 ? ll_ARG_2 : ll_nil;
  ll_THIS->_error = ll_ARGC >= 4 ? ll_ARG_3 : ll_fluid(ll_s(current_error));
  ll_write_barrier_SELF();

  /* Make properties = (error-values <error>) if value == nil */ 
  if ( ll_nullQ(ll_THIS->_properties) && ll_unbox_boolean(ll_THIS->_error) ) {
    ll_THIS->_properties = ll_call(ll_o(properties), _1(ll_THIS->_error));
  }
  
  //ll_format(ll_undef, "<debugger>:initialize ar = ~S\n", 1,ll_ARG_1);
  //ll_format(ll_undef, "<debugger>:initialize = ~S\n", 1, ll_SELF);
  ll_return(ll_SELF);
}
ll_define_primitive_end


ll_define_primitive(debugger, _write_default_contents, _3(obj, port, op), _0())
{
  ll_format(ll_ARG_1, "current-ar ~S top-of-ar-stack ~S properties ~S error ~S", 
	    4,
	    ll_THIS->_current_ar,
	    ll_THIS->_top_of_ar_stack,
	    ll_THIS->_properties,
	    ll_THIS->_error);
}
ll_define_primitive_end


#if 0
ll_define_primitive(object, db, _0(), _0())
{
  ll_return(ll_fluid(ll_s(db)));
}
ll_define_primitive_end
#endif


ll_define_macro(object, db, __0(args))
{
  ll_v fdb = ll_listn(2, ll_s(fluid), ll_s(db));

  if ( ll_ARGC == 0 ) {
    /* (db) => (fluid db) */
    ll_return(fdb);
  } else {
    /* (db <op> . <args>) => (<op> (fluid db) . <args>) */
    ll_v op, args;

    op = ll_ARG_0;

    /* (db (get|set) <var> . <args>) => (db-(get|set) (fluid db) (quote <var>) . <args.) */
    if ( ll_EQ(op, ll_s(get)) || ll_EQ(op, ll_s(set)) ) {
      ll_ARG_1 = ll_listn(2, ll_s(quote), ll_ARG_1);
    }

    op = ll_call(ll_o(symbol__string), _1(op));
    op = ll_call(ll_o(clone), _1(op));
    op = ll_call(ll_o(string_append), _2(ll_make_string("%db-", -1), op));
    op = ll_call(ll_o(string__symbol), _1(op));

    args = ll_listv(ll_ARGC - 1, ll_ARGV + 1);

    ll_return(ll_cons(op, ll_cons(fdb, args)));
  }
}
ll_define_macro_end


/********************************************************************/


ll_define_primitive(debugger, _db_exit, __1(db, value), _0())
{
  if ( ll_ARGC >= 2 ) {
    ll_call_tail(ll_THIS->_exit, _1(ll_ARG_1));
  } else {
    ll_call_tail(ll_THIS->_exit, _0());
  }
}
ll_define_primitive_end


ll_define_primitive(debugger, _db_get_loc, _2(db, var), _0())
{
  ll_call_tail(ll_o(get_arg), _2(ll_THIS->_current_ar, ll_ARG_1));
}
ll_define_primitive_end


ll_define_primitive(debugger, _db_get, _2(db, var), _0())
{
  ll_v loc;

  loc = ll_call(ll_o(get_arg), _2(ll_THIS->_current_ar, ll_ARG_1));
  ll_call_tail(ll_o(contents), _1(loc));
}
ll_define_primitive_end


ll_define_primitive(debugger, _db_set, _3(db, var, value), _0())
{
  ll_v loc;

  loc = ll_call(ll_o(get_arg), _2(ll_THIS->_current_ar, ll_ARG_1));
  ll_call_tail(ll_o(set_contentsE), _2(loc, ll_ARG_2));
}
ll_define_primitive_end


ll_define_primitive(debugger, _db_bt, __1(db, depth), _0())
{
  ll_v depth = ll_ARGC >= 2 ? ll_ARG_1 : ll_BOX_fixnum(-1);
  ll_call_tail(ll_o(print_backtrace), _3(ll_THIS->_top_of_ar_stack, depth, ll_THIS->_current_ar));
}
ll_define_primitive_end


ll_define_primitive(debugger, _db_pf, _1(db), _0())
{
  ll_call_tail(ll_o(print_frame), _1(ll_THIS->_current_ar));
}
ll_define_primitive_end


ll_define_primitive(debugger, _db_up, __1(db, depth), _0())
{
  int n = 1;
  ll_message *ar;

  if ( ll_ARGC >= 2 ) {
    n = ll_unbox_fixnum(ll_ARG_1);
  }
  ar = (ll_message*) ll_UNBOX_ref(ll_THIS->_current_ar);

  ar += n;
  if ( (void*) ar < (void*) ll_UNBOX_ref(ll_THIS->_top_of_ar_stack) ) {
    ar = (void*) ll_UNBOX_ref(ll_THIS->_top_of_ar_stack);
  }
  if ( n == 0 || (void*) ar > (void*) _ll_ar_sp_bottom ) {
    ar = (void*) _ll_ar_sp_bottom;
  }

  ll_THIS->_current_ar = ll_make_ref(ar);
  ll_write_barrier_SELF();

  ll_call_tail(ll_o(print_frame), _1(ll_THIS->_current_ar));
}
ll_define_primitive_end


ll_define_primitive(debugger, _db_down, __1(db, depth), _0())
{
  int n = 1;
  ll_message *ar;

  if ( ll_ARGC >= 2 ) {
    n = ll_unbox_fixnum(ll_ARG_1);
  }
  ar = (ll_message*) ll_UNBOX_ref(ll_THIS->_current_ar);

  ar -= n;
  if ( n == 0 || (void*) ar < (void*) ll_UNBOX_ref(ll_THIS->_top_of_ar_stack) ) {
    ar = (void*) ll_UNBOX_ref(ll_THIS->_top_of_ar_stack);
  }
  if ( (void*) ar > (void*) _ll_ar_sp_bottom ) {
    ar = (void*) _ll_ar_sp_bottom;
  }

  ll_THIS->_current_ar = ll_make_ref(ar);
  ll_write_barrier_SELF();

  ll_call_tail(ll_o(print_frame), _1(ll_THIS->_current_ar));
}
ll_define_primitive_end


ll_define_primitive(debugger, _db_error, _1(db), _1(no_side_effect,"#t"))
{
   ll_return(ll_THIS->_error);
}
ll_define_primitive_end


ll_define_primitive(debugger, _db_help, _1(db), _0())
{
  ll_v err = ll_THIS->_error;

  ll_format(ll_undef, "\n\
==================================================================\n\
\nDebugger help:\n\
\n\
Current error: \n  ~S\n\
", 1, ll_TYPE(err));

  ll_format(ll_undef, "\n\
Properties: \n\
", 0);

  ll_call(ll_o(_write_properties), _2(err, ll_undef));
  
ll_format(ll_undef, "\n\
\n\
  (top-level)\n\
\tReturn to the top-level.\n\
  (db error)\n\
\tGet the error that envoked the debugger.\n\
  (db exit ?<expr>?)\n\
\tReturn from debugger with <expr>.\n\
  (db bt ?n?)\n\
\tPrint a stack backtrace n frames long.\n\
  (db pf)\n\
\tPrint the current stack frame.\n\
  (db up ?n?)\n\
\tGo up the stack n frames.  (db up 0) goes to the root stack frame.\n\
  (db down ?n?)\n\
\tGo down the stack n frames.  (db down 0) goes to the top stack frame.\n\
  (db get var)\n\
\tGet the argument named 'var' from current stack frame.\n\
  (db set var value)\n\
\tSet the argument named 'var' from current stack frame with 'value'.\n\
  (db help)\n\
\tThis help message.\n\
==================================================================\n\
\n\
", 0);
}
ll_define_primitive_end


/************************************************************************/


ll_define_primitive(message, debugger, __1(ar, props), _0())
{
  ll_v ar, props, db;

  ar = ll_SELF;
  props = ll_ARGC >= 2 ? ll_ARG_1 : ll_nil;
  db = ll_call(ll_o(make), _3(ll_type(debugger), ar, props));
  //ll_format(ll_undef, "<ar>:debugger db = ~S\n", 1, db);
  ll_call_tail(ll_o(_db_debug), _1(db));
}
ll_define_primitive_end


ll_define_primitive(object, debugger, __0(ar), _0())
{
  ll_v props = ll_ARGC >= 1 ? ll_SELF : ll_nil;
  ll_call_tail(ll_o(debugger), _2(ll_make_ref(_ll_ar_sp + 1), props));
}
ll_define_primitive_end


ll_define_primitive(error, debugger, _1(err), _0())
{
  ll_v db;

  /* ll_DEBUG_SET(trace, ll_make_fixnum(2)); */
  /* ll_format(ll_undef, "<error>:debugger self = ~S\n", 1, ll_SELF); */
  /* ll_format(ll_undef, "<error>:debugger self->ar = ~S\n", 1, ll_THIS->_ar); */
  db = ll_call(ll_o(make), _4(ll_type(debugger), ll_THIS->_ar, ll_THIS->_properties, ll_SELF));
  /* ll_format(ll_undef, "<error>:debugger db = ~S\n", 1, db); */
  ll_call_tail(ll_o(_db_debug), _1(db));
}
ll_define_primitive_end


/************************************************************************/


ll_define_primitive(debugger, _db_debug, _1(db), _0())
{
  /* The debugger loop */
  ll_v expr, def = ll_unspec, result = ll_unspec;
  ll_v port = ll_undef;

  /* Begin debugger session */
  port = ll_format(port, "ll debugger: type '(db help)' for help.\n", 0);

  /* Print the return action prompt */
  def = ll_call(ll_o(property), _2(ll_SELF, ll_s(return_default)));
  if ( ll_unbox_boolean(def) ) {
    def = ll_cdr(def);
  } else {
    def = ll_unspec;
  }

  expr = ll_call(ll_o(property), _2(ll_SELF, ll_s(return_action)));
  if ( ll_unbox_boolean(expr) ) {
    expr = ll_cdr(expr);
    ll_format(port, "Use (db exit <value>) to ~A (default is ~S)\n", 2, expr, def);
  } else if ( ll_NE(def, ll_unspec) ) {
    expr = ll_unspec;
    ll_format(port, "Use (db exit <value>) to return control (default is ~S)\n", 1, def);
  }
  

  /* Begin a catch for this (db exit) */
  ll_CATCH_BEGIN(ll_type(object), exit);
  {
    ll_THIS->_exit = exit;
    ll_write_barrier_SELF();
    ll_THIS->_level = ll_fluid(ll_s(dbClevel));
    ll_write_barrier_SELF();

    /* (let-fluid (
         (db self)
	 (db:level level + 1)
	 (
	 )
	 ...
    */
    ll_let_fluid();
    {
      /* Use *ll:interactive-input-port* in top-level loop. */
      ll_bind_fluid(ll_s(llCtop_levelCoutput_port), ll_f);
      ll_bind_fluid(ll_s(llCtop_levelCinput_port), ll_f);

      /* Bind (fluid db) to this debugger. */
      ll_bind_fluid(ll_s(db), ll_SELF);
      ll_THIS->_level = ll__ADD(ll_THIS->_level, ll_make_fixnum(1));
      ll_write_barrier_SELF();

      /* Bind (fluid db:level) to this debugger level. */
      ll_bind_fluid(ll_s(dbClevel), ll_THIS->_level);
       
      /* Prompt and read an expression until eof */
      while ( 
	     expr = ll_call(ll_o(llCtop_levelCprompt_read_eval), 
			    _2(ll_s(ll_debug), ll_THIS->_level)),
	     ll_NE(expr, ll_eos)
	     )
	result = expr;
    }
    ll_let_fluid_END();
  }
  ll_CATCH_VALUE(thrown);
  {
    /* (db exit <value>) was called. */
    result = thrown;
  }
  ll_CATCH_END;

  /* If result was not specified, return default. */
  if ( ll_eqQ(result, ll_unspec) ) {
    result = def;
  }

  /* Return result back to caller. */
  ll_return(result);
}
ll_define_primitive_end


/************************************************************************/


ll_v _ll_debug_at_rtn(ll_v *rtnval)
{
  ll_v ar, db, props;

  /* Disable debug at this message. */
  ll_AR_DB_AT_RTN = ll_f;

  ar = ll_make_ref(_ll_ar_sp);
  props = ll_listn(1, ll_cons(ll_s(return_default), *rtnval));
  db = ll_call(ll_o(make), _3(ll_type(debugger), ar, props));
 
  *rtnval = ll_call(ll_o(_db_debug), _1(db));
  return *rtnval;
}


/************************************************************************/


ll_INIT(debugger, 270, "(fluid db)")
{
  ll_bind_fluid(ll_s(db), ll_f);
  ll_bind_fluid(ll_s(dbClevel), ll_make_fixnum(0));

  return 0;
}


/************************************************************************/
